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
    namespace Settings {
        #ifdef VKBP_ENABLE_VALIDATION
        const uint32_t validation_layer_count = 8;
        const char* validation_layer_names[] = {
                "VK_LAYER_GOOGLE_threading",
                "VK_LAYER_LUNARG_mem_tracker",
                "VK_LAYER_LUNARG_object_tracker",
                "VK_LAYER_LUNARG_draw_state",
                "VK_LAYER_LUNARG_param_checker",
                "VK_LAYER_LUNARG_swapchain",
                "VK_LAYER_LUNARG_device_limits",
                "VK_LAYER_LUNARG_image",
                "VK_LAYER_GOOGLE_unique_objects",
        };
        #else
        const uint32_t validation_layer_count = 0;
        const char* validation_layer_names = NULL;
        #endif
        #ifdef WIN32
        const char* windowing_system_extension_name = VK_KHR_WIN32_SURFACE_EXTENSION_NAME;
        #else
        const char* windowing_system_extension_name = VK_KHR_XCB_SURFACE_EXTENSION_NAME;
        #endif
    }
}