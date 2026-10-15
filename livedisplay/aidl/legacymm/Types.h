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

#ifndef VENDOR_LINEAGE_LIVEDISPLAY_AIDL_LEGACYMM_TYPES_H
#define VENDOR_LINEAGE_LIVEDISPLAY_AIDL_LEGACYMM_TYPES_H

#include <cstdint>

namespace aidl::vendor::lineage::livedisplay::legacymm {

struct mm_cb_range {
    int max;
    int min;
};

struct mm_disp_mode {
    int id;
    char* name;
    uint32_t len;
    int32_t type;
};

struct mm_pa_data {
    int hue;
    int saturation;
    int intensity;
    int contrast;
    int saturationThreshold;
};

struct mm_pa_config {
    int flags;
    struct mm_pa_data data;
};

struct mm_pa_range {
    struct mm_pa_data max;
    struct mm_pa_data min;
};

}  // namespace aidl::vendor::lineage::livedisplay::legacymm

#endif  // VENDOR_LINEAGE_LIVEDISPLAY_AIDL_LEGACYMM_TYPES_H
