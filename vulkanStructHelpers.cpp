/**
 * part of VKBP (Vulkan Boilerplate)
 *
 * Galen Cochrane
 * Copyright (C) 2016
 * This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
 */

#include <vector>
#include "vulkan/vulkan.h"
#include "vkbpDefines.h"
#include "vulkanStructHelpers.h"

namespace vkbp {

    int FillVkApplicationInfo(VkApplicationInfo &appInfo) {
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "VulkanBoilerplate";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "VulkanBoilerplate";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION;
        return 0;
    }

    int FillVkInstanceInfo(const VkApplicationInfo& appInfo, VkInstanceCreateInfo& instanceInfo) {
        std::vector<const char*> vkExtensionsToUse;
        vkExtensionsToUse.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
        vkExtensionsToUse.push_back(surface_extension_name);
        if (VKBP_ENABLE_VALIDATION) {
            vkExtensionsToUse.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
        }

        instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instanceInfo.pNext = NULL;
        instanceInfo.flags = 0;
        instanceInfo.pApplicationInfo = &appInfo;
        instanceInfo.enabledLayerCount = (uint32_t)vkExtensionsToUse.size();
        instanceInfo.ppEnabledLayerNames = vkExtensionsToUse.data();
        instanceInfo.enabledExtensionCount
        return 0;
    }

}
