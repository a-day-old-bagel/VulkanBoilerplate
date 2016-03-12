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
#include "vkbpGlobal.h"

namespace vkbp {
    struct VulkanContext {
        VkInstance instance;
        bool hasInit;

        VulkanContext();
        ~VulkanContext();
        VkResult init(const char* appName, const char* engName);
    };
}

#endif //VKBP_VULKANCONTEXT_H
