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
#include "vulkanHelpers.h"

namespace vkbp {
    struct Vk {
        VkSurfaceKHR surface;
        VkInstance instance;
        VkPhysicalDevice physicalDevice;
//        VkPhysicalDeviceMemoryProperties deviceMemoryProperties;
        VkDevice device;
        VkQueue queue;
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

        Vk();
        ~Vk();
        VkbpResult init(const char* appName, const char* engName);
        VkbpResult initSimple();
    };
}

#endif //VKBP_VULKANCONTEXT_H
