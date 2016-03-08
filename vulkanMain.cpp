//
// Created by Galen on 3/8/2016.
//

#include "vulkan.h"

int main() {
    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "VulkanBoilerplate";
    appInfo.pEngineName = "VulkanBoilerplate";
    appInfo.apiVersion = VK_API_VERSION;
    return 0;
}