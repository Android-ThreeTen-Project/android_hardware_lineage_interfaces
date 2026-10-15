// SPDX-License-Identifier: Apache-2.0
// Adapt the legacy Qualcomm memtrack module to the framework's AIDL transport.
#include <aidl/android/hardware/memtrack/BnMemtrack.h>
#include <android/binder_manager.h>
#include <android/binder_process.h>
#include <hardware/hardware.h>
#include <hardware/memtrack.h>
#include <log/log.h>
#include <unistd.h>

#include <cerrno>
#include <mutex>
#include <string>
#include <vector>

namespace aidl::android::hardware::memtrack {
class LegacyMemtrack final : public BnMemtrack {
  public:
    explicit LegacyMemtrack(const memtrack_module_t* module) : mModule(module) {
        if (mModule->init) mModule->init(mModule);
    }

    ndk::ScopedAStatus getMemory(int32_t pid, MemtrackType type,
                                 std::vector<MemtrackRecord>* records) override {
        records->clear();
        if (pid < 0) return ndk::ScopedAStatus::fromExceptionCode(EX_ILLEGAL_ARGUMENT);
        if (!mModule->getMemory)
            return ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
        std::lock_guard lock(mMutex);
        size_t count = 0;
        int rc = mModule->getMemory(mModule, pid, static_cast<int>(type), nullptr, &count);
        if (rc != 0) return statusFor(rc);
        std::vector<memtrack_record> legacy(count);
        const size_t capacity = count;
        rc = mModule->getMemory(mModule, pid, static_cast<int>(type), legacy.data(), &count);
        if (rc != 0) return statusFor(rc);
        if (count > capacity) return ndk::ScopedAStatus::fromServiceSpecificError(EOVERFLOW);
        records->reserve(count);
        for (size_t i = 0; i < count; ++i) {
            MemtrackRecord record;
            record.flags = legacy[i].flags;
            record.sizeInBytes = legacy[i].size_in_bytes;
            records->push_back(record);
        }
        return ndk::ScopedAStatus::ok();
    }

    ndk::ScopedAStatus getGpuDeviceInfo(std::vector<DeviceInfo>* devices) override {
        devices->clear();
        if (access("/sys/class/kgsl/kgsl-3d0", F_OK) != 0)
            return ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
        DeviceInfo device;
        device.id = 0;
        device.name = "kgsl-3d0";
        devices->push_back(device);
        return ndk::ScopedAStatus::ok();
    }

  private:
    static ndk::ScopedAStatus statusFor(int rc) {
        if (rc == -EINVAL || rc == -ENOSYS)
            return ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
        return ndk::ScopedAStatus::fromServiceSpecificError(rc < 0 ? -rc : rc);
    }
    const memtrack_module_t* mModule;
    std::mutex mMutex;
};
}  // namespace aidl::android::hardware::memtrack

int main() {
    const hw_module_t* hardware = nullptr;
    if (hw_get_module(MEMTRACK_HARDWARE_MODULE_ID, &hardware) != 0) {
        ALOGE("Cannot load legacy memtrack HAL");
        return 1;
    }
    const memtrack_module_t* module = reinterpret_cast<const memtrack_module_t*>(hardware);
    if (hardware->methods && hardware->methods->open) {
        hw_device_t* device = nullptr;
        if (hardware->methods->open(hardware, MEMTRACK_HARDWARE_MODULE_ID, &device) != 0) return 1;
        module = reinterpret_cast<const memtrack_module_t*>(device);
    }
    ABinderProcess_setThreadPoolMaxThreadCount(0);
    auto service = ndk::SharedRefBase::make<aidl::android::hardware::memtrack::LegacyMemtrack>(module);
    const std::string instance = std::string(service->descriptor) + "/default";
    if (AServiceManager_addService(service->asBinder().get(), instance.c_str()) != STATUS_OK) return 1;
    ABinderProcess_joinThreadPool();
    return 1;
}
