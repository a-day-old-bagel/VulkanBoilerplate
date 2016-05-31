/**
 * part of VKBP (Vulkan Boilerplate)
 *
 * Galen Cochrane
 * Copyright (C) 2016
 * This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
 */
#ifndef VKBP_VULKANCONTEXT_H
#define VKBP_VULKANCONTEXT_H

#include "vulkan/vulkan.h"
#include "vkbpGlobal.h"
#include "vkbpHelpers.h"

namespace vkbp {

    struct SwapChainBuffer {
        VkImage image;
        VkImageView view;
    };

    struct Vkbp {
        VkSurfaceKHR surface;

        VkInstance instance;
        VkPhysicalDevice physicalDevice;
        VkDevice device;
        VkQueue queue;

        VkCommandBuffer setupCmdBuffer;

        VkSwapchainKHR swapChain;
        uint32_t width, height, imageCount;
        std::vector<SwapChainBuffer> buffers;
        std::vector<VkImage> images;

        bool hasInit;

        PFN_vkGetPhysicalDeviceSurfaceSupportKHR fpGetPhysicalDeviceSurfaceSupportKHR;
        PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR fpGetPhysicalDeviceSurfaceCapabilitiesKHR;
        PFN_vkGetPhysicalDeviceSurfaceFormatsKHR fpGetPhysicalDeviceSurfaceFormatsKHR;
        PFN_vkGetPhysicalDeviceSurfacePresentModesKHR fpGetPhysicalDeviceSurfacePresentModesKHR;
        PFN_vkCreateSwapchainKHR fpCreateSwapchainKHR;
        PFN_vkDestroySwapchainKHR fpDestroySwapchainKHR;
        PFN_vkGetSwapchainImagesKHR fpGetSwapchainImagesKHR;
        PFN_vkAcquireNextImageKHR fpAcquireNextImageKHR;
        PFN_vkQueuePresentKHR fpQueuePresentKHR;
        PFN_vkCreateDebugReportCallbackEXT CreateDebugReportCallback;
        PFN_vkDestroyDebugReportCallbackEXT DestroyDebugReportCallback;
        PFN_vkDebugReportMessageEXT dbgBreakCallback;

        Vkbp();
        ~Vkbp();
        VkbpResult init(const char* appName, const char* engName);
        VkbpResult initSimple();
        VkbpResult initSimple2();
    };
}

#endif //VKBP_VULKANCONTEXT_H
