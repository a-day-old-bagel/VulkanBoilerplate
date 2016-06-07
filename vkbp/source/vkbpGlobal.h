/**
 * part of VKBP (Vulkan Boilerplate)
 *
 * Galen Cochrane
 * Copyright (C) 2016
 * This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
 */

#ifndef VKBP_DEFINES_H
#define VKBP_DEFINES_H

#include <vector>

// The NDEBUG switch should be defined for any release build.  It removes validation layers and other debug code.
#ifndef NDEBUG
#define VKBP_ENABLE_VALIDATION
#endif

#ifdef VDEBUG
#define VKBP_VERBOSE_DEBUG true
#else
#define VKBP_VERBOSE_DEBUG false
#endif

namespace vkbp {
    namespace Debug {

        extern const std::vector<const char*> validationLayerNames;

//        extern uint32_t validationLayerCount;
//#ifdef VKBP_ENABLE_VALIDATION
//        extern const char* validationLayerNames[];
//#else
//        extern const char* const* validationLayerNames;
//#endif

    }
    namespace Settings {

    }
}

#endif //VKBP_DEFINES_H
