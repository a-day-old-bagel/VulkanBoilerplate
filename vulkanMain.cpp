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

    vkbp::VkbpResult res = vk.initSimple();
    if (res.isError()) {
        std::cout << res.toString() << std::endl;
        std::cout << "Vulkan failed to initialize!\n";
        return 1;
    } else {
        std::cout << std::endl << "Vulkan instance created." << std::endl;
    }

    return 0;
}