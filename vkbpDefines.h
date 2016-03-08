/**
 * part of VKBP (Vulkan Boilerplate)
 *
 * Galen Cochrane
 * Copyright (C) 2016
 * This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
 */

#ifndef VKBP_DEFINES_H
#define VKBP_DEFINES_H

#ifndef NDEBUG
#define VKBP_ENABLE_VALIDATION
#endif

namespace vkbp {
    namespace settings {
        extern const char* windowing_system_extension_name;
        extern const uint32_t validation_layer_count;
        extern const char* validation_layer_names[];
    }
}

#endif //VKBP_DEFINES_H
