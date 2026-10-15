/*
 * SPDX-FileCopyrightText: 2019-2026 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#define LOG_TAG "vendor.lineage.livedisplay-service.legacymm"
#include <android-base/logging.h>
#include <android-base/scopeguard.h>
#include <android/binder_manager.h>
#include <android/binder_process.h>
#include <dlfcn.h>
#include <memory>

#include "DisplayModes.h"
#include "PictureAdjustment.h"

using aidl::vendor::lineage::livedisplay::legacymm::DisplayModes;
using aidl::vendor::lineage::livedisplay::legacymm::PictureAdjustment;

int main() {
    ABinderProcess_setThreadPoolMaxThreadCount(1);
    ABinderProcess_startThreadPool();
    std::unique_ptr<void, decltype(&dlclose)> lib(dlopen("libmm-disp-apis.so", RTLD_NOW), dlclose);
    if (!lib) {
        LOG(ERROR) << "Cannot load libmm-disp-apis.so: " << dlerror();
        return 1;
    }
    auto init = reinterpret_cast<int (*)(int32_t)>(dlsym(lib.get(), "disp_api_init"));
    if (!init || init(0) != 0) {
        LOG(ERROR) << "Cannot initialize libmm-disp-apis.so";
        return 1;
    }
    auto cleanup = android::base::make_scope_guard([&] { init(1); });
    auto dm = ndk::SharedRefBase::make<DisplayModes>(lib.get());
    auto pa = ndk::SharedRefBase::make<PictureAdjustment>(lib.get());
    if (!dm->isSupported() && !pa->isSupported()) return 1;

    // Declared interfaces are registered even if one feature is unavailable;
    // that feature returns UNSUPPORTED_OPERATION instead of blocking its client.
    const std::string dmName = std::string(DisplayModes::descriptor) + "/default";
    const std::string paName = std::string(PictureAdjustment::descriptor) + "/default";
    CHECK_EQ(AServiceManager_addService(dm->asBinder().get(), dmName.c_str()), STATUS_OK);
    CHECK_EQ(AServiceManager_addService(pa->asBinder().get(), paName.c_str()), STATUS_OK);
    ABinderProcess_joinThreadPool();
    return 1;
}
