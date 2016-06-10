/**
 * Galen Cochrane
 * Copyright (C) 2016
 * This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
 */

#include <vector>
#include <iostream>
#include "vkbp/vkbp.h"

using namespace vkbp;

int main() {

    Vkbp vk;
    VkbpResult res = vk.initAllInOne();
    if (res.isError()) {
        std::cout << res.toString() << std::endl;
        return res.errCode;
    }

    return 0;
}