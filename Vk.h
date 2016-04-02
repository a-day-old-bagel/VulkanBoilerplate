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
    struct Vk {
        VkInstance instance;
        VkPhysicalDevice physicalDevice;
        VkDevice device;
        VkQueue queue;
        bool hasInit;

        Vk();
        ~Vk();
        VkResult init(const char* appName, const char* engName);
        VkResult initSimple();
    };
}

#endif //VKBP_VULKANCONTEXT_H
