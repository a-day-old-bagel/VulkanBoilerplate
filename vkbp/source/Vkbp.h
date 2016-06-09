/**
 * part of VKBP (Vulkan Boilerplate)
 *
 * Galen Cochrane
 * Copyright (C) 2016
 * This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
 */
#ifndef VKBP_VULKANCONTEXT_H
#define VKBP_VULKANCONTEXT_H

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"
#include "vulkan/vulkan.h"
#include "vkbpGlobal.h"
#include "vkbpHelpers.h"
#include "VkbpValidator.h"

namespace vkbp {

    struct SwapChainBuffer {
        VkImage image;
        VkImageView view;
    };

    struct Vkbp {
        Vkbp();
        ~Vkbp();
        VkbpResult init(const char* appName, const char* engName);
        VkbpResult initAllInOne();
    };
}

#endif //VKBP_VULKANCONTEXT_H
