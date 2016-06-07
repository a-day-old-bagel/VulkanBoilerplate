/**
 * part of VKBP (Vulkan Boilerplate)
 *
 * Galen Cochrane
 * Copyright (C) 2016
 * This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
 */

#include "vulkan/vulkan.h"
#include "vkbpGlobal.h"

namespace vkbp {
    namespace Debug {

#ifdef VKBP_ENABLE_VALIDATION
        const std::vector<const char*> validationLayerNames = { "VK_LAYER_LUNARG_standard_validation" };
//        uint32_t validationLayerCount = 1;
//        const char* validationLayerNames[] = {
//                "VK_LAYER_LUNARG_standard_validation"   // includes a bunch of commonly-used layers
//        };
#else
        const std::vector<const char*> validationLayerNames;
//        uint32_t validationLayerCount = 0;
//        const char* const* validationLayerNames = NULL;
#endif

    }
    namespace Settings {

    }
}