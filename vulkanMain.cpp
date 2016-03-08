/**
 * Galen Cochrane
 * Copyright (C) 2016
 * This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
 */

#include <vector>
#include <iostream>
#include "vkbp.h"

int main() {

    vkbp::VulkanContext vk;

    VkResult res = vk.init("Vulkan Test", "VKBP");
    if (res != VK_SUCCESS) {
        std::cout << std::endl << "Vulkan failed to initialize: " << vkbp::errorString(res) << std::endl;
        return 1;
    }

    return 0;
}