/**
 * Galen Cochrane
 * Copyright (C) 2016
 * This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
 */

#include "vulkan/vulkan.h"
#include "vkbpDefines.h"
#include "VulkanContext.h"
#include "vulkanStructHelpers.h"

using namespace vkbp;

int main() {

    VulkanContext vk;

    VkApplicationInfo appInfo;
    FillVkApplicationInfo(appInfo);

    VkInstanceCreateInfo instanceInfo;
    FillVkInstanceInfo(appInfo, instanceInfo);

    vk.init(instanceInfo);

    return 0;
}