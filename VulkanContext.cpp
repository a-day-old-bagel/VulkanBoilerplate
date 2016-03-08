/**
 * part of VKBP (Vulkan Boilerplate)
 *
 * Galen Cochrane
 * Copyright (C) 2016
 * This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
 */

#include "vulkan/vulkan.h"
#include "VulkanContext.h"
#include "vulkanHelpers.h"

namespace vkbp {

    VulkanContext::VulkanContext() {
        instance = 0;
        hasInit = false;
    }

    VulkanContext::~VulkanContext() {
        if (hasInit) {
            vkDestroyInstance(instance, NULL);
        }
    }

    VkResult VulkanContext::init(const char* appName, const char* engName) {

        VkApplicationInfo appInfo;
        FillVkApplicationInfo(appName, engName, appInfo);

        std::vector<const char*> vkExtensionsToUse;
        AppendBasicVkExtensions(vkExtensionsToUse);

        VkInstanceCreateInfo instanceInfo;
        FillVkInstanceInfo(appInfo, vkExtensionsToUse, instanceInfo);

        VkResult res = vkCreateInstance(&instanceInfo, NULL, &instance);
        if (res == VK_SUCCESS) {
            hasInit = true;
        }
        return res;
    }

}
