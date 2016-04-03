/**
 * Galen Cochrane
 * Copyright (C) 2016
 * This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
 */

#include <vector>
#include <iostream>
#include "vkbp.h"

int main() {

    vkbp::Vk vk;

    VkResult res = vk.initSimple();
    if (res != VK_SUCCESS) {
        std::cout << std::endl << "Vulkan failed to initialize: " << vkbp::resolveErrorToString(res) << std::endl;
        return 1;
    } else {
        std::cout << std::endl << "Vulkan instance created." << std::endl;
    }

    return 0;
}