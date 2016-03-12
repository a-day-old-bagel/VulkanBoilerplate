/**
 * part of VKBP (Vulkan Boilerplate)
 *
 * Galen Cochrane
 * Copyright (C) 2016
 * This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
 */

#ifndef VKBP_DEFINES_H
#define VKBP_DEFINES_H

// The NDEBUG switch should be defined for any release build.  It removes validation layers and other debug code.
#ifndef NDEBUG
#define VKBP_ENABLE_VALIDATION
#endif

namespace vkbp {
    namespace settings {
        extern const char* windowing_system_extension_name;
        extern const uint32_t validation_layer_count;
        #ifdef VKBP_ENABLE_VALIDATION
        extern const char* validation_layer_names[];
        #else
        extern const char* validation_layer_names;
        #endif
    }
}

#endif //VKBP_DEFINES_H
