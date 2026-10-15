/*
 * Copyright (C) 2019 The LineageOS Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "PictureAdjustment.h"

#include <dlfcn.h>

#include "Constants.h"
#include "Types.h"

namespace aidl::vendor::lineage::livedisplay::legacymm {

// Owned by the service for its lifetime.
static PictureAdjustment* sInstance = nullptr;

PictureAdjustment::PictureAdjustment(void* libHandle) {
    sInstance = this;

    mLibHandle = libHandle;
    disp_api_supported =
            reinterpret_cast<int (*)(int32_t, int32_t)>(dlsym(mLibHandle, "disp_api_supported"));
    disp_api_get_pa_range =
            reinterpret_cast<int (*)(int32_t, void*)>(dlsym(mLibHandle, "disp_api_get_pa_range"));
    disp_api_get_pa_config =
            reinterpret_cast<int (*)(int32_t, void*)>(dlsym(mLibHandle, "disp_api_get_pa_config"));
    disp_api_set_pa_config =
            reinterpret_cast<int (*)(int32_t, void*)>(dlsym(mLibHandle, "disp_api_set_pa_config"));
    mDefaultPictureAdjustment = {};
}

bool PictureAdjustment::isSupported() {
    mm_pa_range r{};

    if (disp_api_supported == nullptr || disp_api_supported(0, PICTURE_ADJUSTMENT_FEATURE) == 0) {
        return false;
    }

    if (disp_api_get_pa_range == nullptr || disp_api_get_pa_range(0, &r) != 0) {
        return false;
    }

    return r.max.hue != 0 && r.min.hue != 0 && r.max.saturation != 0.f && r.min.saturation != 0.f &&
           r.max.intensity != 0.f && r.min.intensity != 0.f && r.max.contrast != 0.f &&
           r.min.contrast != 0.f;
}

HSIC PictureAdjustment::getPictureAdjustmentInternal() {
    mm_pa_config config{};

    if (disp_api_get_pa_config != nullptr) {
        if (disp_api_get_pa_config(0, &config) == 0) {
            return HSIC{static_cast<float>(config.data.hue),
                        static_cast<float>(config.data.saturation),
                        static_cast<float>(config.data.intensity),
                        static_cast<float>(config.data.contrast),
                        static_cast<float>(config.data.saturationThreshold)};
        }
    }

    return HSIC{};
}

void PictureAdjustment::updateDefaultPictureAdjustment() {
    std::lock_guard<std::recursive_mutex> lock(gBackendMutex);
    if (sInstance != nullptr) {
        sInstance->mDefaultPictureAdjustment = sInstance->getPictureAdjustmentInternal();
    }
}

// Methods from ::aidl::vendor::lineage::livedisplay::BnPictureAdjustment follow.
ndk::ScopedAStatus PictureAdjustment::getHueRange(FloatRange* _aidl_return) {
    std::lock_guard<std::recursive_mutex> lock(gBackendMutex);
    if (!isSupported()) return ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
    FloatRange range{};
    mm_pa_range r{};

    if (disp_api_get_pa_range != nullptr) {
        if (disp_api_get_pa_range(0, &r) == 0) {
            range.max = r.max.hue;
            range.min = r.min.hue;
            range.step = 1;
        }
    }

    *_aidl_return = range;
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus PictureAdjustment::getSaturationRange(FloatRange* _aidl_return) {
    std::lock_guard<std::recursive_mutex> lock(gBackendMutex);
    if (!isSupported()) return ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
    FloatRange range{};
    mm_pa_range r{};

    if (disp_api_get_pa_range != nullptr) {
        if (disp_api_get_pa_range(0, &r) == 0) {
            range.max = r.max.saturation;
            range.min = r.min.saturation;
            range.step = 1;
        }
    }

    *_aidl_return = range;
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus PictureAdjustment::getIntensityRange(FloatRange* _aidl_return) {
    std::lock_guard<std::recursive_mutex> lock(gBackendMutex);
    if (!isSupported()) return ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
    FloatRange range{};
    mm_pa_range r{};

    if (disp_api_get_pa_range != nullptr) {
        if (disp_api_get_pa_range(0, &r) == 0) {
            range.max = r.max.intensity;
            range.min = r.min.intensity;
            range.step = 1;
        }
    }

    *_aidl_return = range;
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus PictureAdjustment::getContrastRange(FloatRange* _aidl_return) {
    std::lock_guard<std::recursive_mutex> lock(gBackendMutex);
    if (!isSupported()) return ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
    FloatRange range{};
    mm_pa_range r{};

    if (disp_api_get_pa_range != nullptr) {
        if (disp_api_get_pa_range(0, &r) == 0) {
            range.max = r.max.contrast;
            range.min = r.min.contrast;
            range.step = 1;
        }
    }

    *_aidl_return = range;
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus PictureAdjustment::getSaturationThresholdRange(FloatRange* _aidl_return) {
    std::lock_guard<std::recursive_mutex> lock(gBackendMutex);
    if (!isSupported()) return ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
    FloatRange range{};
    mm_pa_range r{};

    if (disp_api_get_pa_range != nullptr) {
        if (disp_api_get_pa_range(0, &r) == 0) {
            range.max = r.max.saturationThreshold;
            range.min = r.min.saturationThreshold;
            range.step = 1;
        }
    }

    *_aidl_return = range;
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus PictureAdjustment::getPictureAdjustment(HSIC* _aidl_return) {
    std::lock_guard<std::recursive_mutex> lock(gBackendMutex);
    if (!isSupported()) return ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
    *_aidl_return = getPictureAdjustmentInternal();
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus PictureAdjustment::getDefaultPictureAdjustment(HSIC* _aidl_return) {
    std::lock_guard<std::recursive_mutex> lock(gBackendMutex);
    if (!isSupported()) return ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
    *_aidl_return = mDefaultPictureAdjustment;
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus PictureAdjustment::setPictureAdjustment(
        const HSIC& hsic) {
    std::lock_guard<std::recursive_mutex> lock(gBackendMutex);
    if (!isSupported()) return ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
    mm_pa_config config = {0xF,
                           {static_cast<int>(hsic.hue), static_cast<int>(hsic.saturation),
                            static_cast<int>(hsic.intensity), static_cast<int>(hsic.contrast),
                            static_cast<int>(hsic.saturationThreshold)}};

    if (disp_api_set_pa_config != nullptr) {
        return disp_api_set_pa_config(0, &config) == 0
                       ? ndk::ScopedAStatus::ok()
                       : ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
    }

    return ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
}

}  // namespace aidl::vendor::lineage::livedisplay::legacymm
