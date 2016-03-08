/**
 * part of VKBP (Vulkan Boilerplate)
 *
 * Galen Cochrane
 * Copyright (C) 2016
 * This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
 */

#include "vulkan/vulkan.h"
#include "vkbpDefines.h"

namespace vkbp {
    #ifdef _WIN32
    const char* surface_extension_name = VK_KHR_WIN32_SURFACE_EXTENSION_NAME;
    #else
    const char* surface_extension_name = VK_KHR_XCB_SURFACE_EXTENSION_NAME;
    #endif
}