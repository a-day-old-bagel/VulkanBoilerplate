/**
 * part of VKBP (Vulkan Boilerplate)
 *
 * Galen Cochrane
 * Copyright (C) 2016
 * This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
 */
#ifndef VKBP_VULKANCONTEXT_H
#define VKBP_VULKANCONTEXT_H

#include "vulkan/vulkan.h"
#include "vkbpDefines.h"

namespace vkbp {
    struct VulkanContext {
        VkInstance instance;

        VulkanContext();
        ~VulkanContext();
        int init(VkInstanceCreateInfo& instanceInfo);
    };
}

#endif //VKBP_VULKANCONTEXT_H
