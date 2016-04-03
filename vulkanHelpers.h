/**
 * part of VKBP (Vulkan Boilerplate)
 *
 * Galen Cochrane
 * Copyright (C) 2016
 * This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
 */

#include <vector>
#include <string>
#include "vulkan/vulkan.h"
#include "vkbpGlobal.h"

#ifndef VKBP_VULKANSTRUCTHELPERS_H
#define VKBP_VULKANSTRUCTHELPERS_H

namespace vkbp {
    void AppendBasicVkExtensions(std::vector<const char*> &extVec);
    void FillVkApplicationInfo(const char* appName, const char* engName, VkApplicationInfo &appInfo);
    void FillVkInstanceInfo(const VkApplicationInfo &appInfo, const std::vector<const char*> &extVec,
                            VkInstanceCreateInfo &instanceInfo);
    std::string resolveErrorToString(VkResult err);
    std::string resolvePhysicalDeviceTypeToString(VkPhysicalDeviceType type);
}

#endif //VKBP_VULKANSTRUCTHELPERS_H
