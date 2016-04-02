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

    VkResult res = vk.init("Vulkan Test", "VKBP");
    if (res != VK_SUCCESS) {
        std::cout << std::endl << "Vulkan failed to initialize: " << vkbp::resolveEnumToString(res) << std::endl;
        return 1;
    } else {
        std::cout << std::endl << "Vulkan instance created." << std::endl;
    }

    if (vkbp::settings::validation_layer_count) {
        std::cout << "Vulkan validation layers active." << std::endl;
    }

    return 0;
}