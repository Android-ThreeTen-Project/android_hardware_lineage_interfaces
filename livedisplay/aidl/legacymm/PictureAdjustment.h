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

#ifndef VENDOR_LINEAGE_LIVEDISPLAY_AIDL_PICTUREADJUSTMENT_H
#define VENDOR_LINEAGE_LIVEDISPLAY_AIDL_PICTUREADJUSTMENT_H

#include <aidl/vendor/lineage/livedisplay/BnPictureAdjustment.h>
#include <vector>

namespace aidl::vendor::lineage::livedisplay::legacymm {


class PictureAdjustment : public BnPictureAdjustment {
  public:
    PictureAdjustment(void* libHandle);

    bool isSupported();

    // Methods from ::aidl::vendor::lineage::livedisplay::BnPictureAdjustment
    // follow.
    ndk::ScopedAStatus getHueRange(FloatRange* _aidl_return) override;
    ndk::ScopedAStatus getSaturationRange(FloatRange* _aidl_return) override;
    ndk::ScopedAStatus getIntensityRange(FloatRange* _aidl_return) override;
    ndk::ScopedAStatus getContrastRange(FloatRange* _aidl_return) override;
    ndk::ScopedAStatus getSaturationThresholdRange(FloatRange* _aidl_return) override;
    ndk::ScopedAStatus getPictureAdjustment(HSIC* _aidl_return) override;
    ndk::ScopedAStatus getDefaultPictureAdjustment(HSIC* _aidl_return) override;
    ndk::ScopedAStatus setPictureAdjustment(
            const HSIC& hsic) override;

    static void updateDefaultPictureAdjustment();

  private:
    void* mLibHandle;

    int (*disp_api_supported)(int32_t, int32_t);
    int (*disp_api_get_pa_range)(int32_t, void*);
    int (*disp_api_get_pa_config)(int32_t, void*);
    int (*disp_api_set_pa_config)(int32_t, void*);

    HSIC getPictureAdjustmentInternal();

    HSIC mDefaultPictureAdjustment;
};

}  // namespace aidl::vendor::lineage::livedisplay::legacymm

#endif  // VENDOR_LINEAGE_LIVEDISPLAY_AIDL_PICTUREADJUSTMENT_H
