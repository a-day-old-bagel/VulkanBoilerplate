/**
 * part of VKBP (Vulkan Boilerplate)
 *
 * Galen Cochrane
 * Copyright (C) 2016
 * This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
 */

#include "vulkan/vulkan.h"
#include "VulkanContext.h"

namespace vkbp {

    VulkanContext::VulkanContext() {

    }

    VulkanContext::~VulkanContext() {

    }

    int VulkanContext::init(VkInstanceCreateInfo &instanceInfo) {
        vkCreateInstance(&instanceInfo, NULL, &instance);
        return 0;
    }

}
