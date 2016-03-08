/**
 * part of VKBP (Vulkan Boilerplate)
 *
 * Galen Cochrane
 * Copyright (C) 2016
 * This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
 */

#include "vulkan/vulkan.h"
#include "vkbpDefines.h"

#ifndef VKBP_VULKANSTRUCTHELPERS_H
#define VKBP_VULKANSTRUCTHELPERS_H

namespace vkbp {
    int FillVkApplicationInfo(VkApplicationInfo& appInfo);
    int FillVkInstanceInfo(const VkApplicationInfo &appInfo, VkInstanceCreateInfo &instanceInfo);
}

#endif //VKBP_VULKANSTRUCTHELPERS_H
