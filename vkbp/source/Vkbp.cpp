/**
 * part of VKBP (Vulkan Boilerplate)
 *
 * Galen Cochrane
 * Copyright (C) 2016
 * This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
 */

#include <array>
#include <iostream>
#include <cstring>
#include <sstream>
#include "Vkbp.h"

namespace vkbp {

    Vkbp::Vkbp() {

    }

    Vkbp::~Vkbp() {

    }

    VkbpResult Vkbp::init(const char* appName, const char* engName) {
        return VKBP_SUCCESS;
    }

    VkbpResult Vkbp::initAllInOne() {
        VkbpResult returnValue = VKBP_SUCCESS;
        VkResult result;

        //region VulkanBasicMembers

        VkInstance instance = VK_NULL_HANDLE;
        VkPhysicalDevice physDevice = VK_NULL_HANDLE;
        VkDevice logiDevice = VK_NULL_HANDLE;
        VkQueue queue = VK_NULL_HANDLE;
        uint32_t graphicsFamilyIndex = 0;
        std::vector<const char*> instanceExtensions;
        std::vector<const char*> deviceExtensions;
        VkbpValidator validator;

        //endregion
        //region WindowMembers

        VkSurfaceKHR surface = VK_NULL_HANDLE;
        VkSwapchainKHR swapchain = VK_NULL_HANDLE;
        uint32_t surfaceSizeX = 512;
        uint32_t surfaceSizeY = 512;
        std::string windowName;
        uint32_t swapchainImageCount = 2;
        std::vector<VkImage> swapchainImages;
        std::vector<VkImageView> swapchainImageViews;
        VkSurfaceFormatKHR surfaceFormat = {};
        VkSurfaceCapabilitiesKHR surfaceCapabilities = {};
        GLFWwindow* glfwWindow = nullptr;

        //endregion

        //region InitGlfw

        glfwInit();
        if (glfwVulkanSupported() == GLFW_FALSE) {
            return VKBP_MSG("GLFW Vulkan support not found.");
        }

        //endregion
        //region DetermineExtensions

        instanceExtensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
        uint32_t instanceExtensionsCount = 0;
        const char** instanceExtensionsBuffer = glfwGetRequiredInstanceExtensions(&instanceExtensionsCount);
        for (uint32_t i = 0; i < instanceExtensionsCount; i++) {
            instanceExtensions.push_back(instanceExtensionsBuffer[i]);
        }
        deviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

        //endregion
        //region InitInstance

        VkApplicationInfo applicationInfo {};
        applicationInfo.sType               =   VK_STRUCTURE_TYPE_APPLICATION_INFO ;
        applicationInfo.apiVersion          =             VK_MAKE_VERSION(1, 0, 2) ;
        applicationInfo.applicationVersion  =             VK_MAKE_VERSION(0, 1, 0) ;
        applicationInfo.pApplicationName    =                 "Vulkan Boilerplate" ;

        VkInstanceCreateInfo instanceCreateInfo {};
        instanceCreateInfo.sType                    =         VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO ;
        instanceCreateInfo.pApplicationInfo         =                               &applicationInfo ;
        instanceCreateInfo.enabledExtensionCount    =           (uint32_t) instanceExtensions.size() ;
        instanceCreateInfo.ppEnabledExtensionNames  =                      instanceExtensions.data() ;
        instanceCreateInfo.pNext                    =                                           NULL ;

        VkbpResult res = validator.addValidationInfoBeforeInstanceCreation(instanceExtensions, instanceCreateInfo);
        if (res.isError()) { return res; }

        result = vkCreateInstance(&instanceCreateInfo, nullptr, &instance);
        VKBP_CHECK_ERR(result);

        res = validator.createDebugReportCallbackAfterInstanceCreation(instance);
        if (res.isError()) { return res; }

        //endregion
        //region FindGpuAndGraphicsQueue

        std::stringstream report;
        uint32_t physDeviceCount = 0;
        result = vkEnumeratePhysicalDevices(instance, &physDeviceCount, nullptr);
        VKBP_CHECK_ERR(result);
        if (!physDeviceCount) {
            return VKBP_MSG("No physical device found!");
        }
        std::vector<VkPhysicalDevice> physDeviceList(physDeviceCount);
        result = vkEnumeratePhysicalDevices(instance, &physDeviceCount, physDeviceList.data());
        VKBP_CHECK_ERR(result);
        VkPhysicalDeviceProperties physDevProperties {};
        std::vector<uint32_t> gpuList, integratedGpuList;
        report << "Physical devices recognized: (" << physDeviceCount << " total): \n    ----\n";
        for (uint32_t i = 0; i < physDeviceCount; ++i) {
            vkGetPhysicalDeviceProperties(physDeviceList[i], &physDevProperties);
            report << "    NAME: " << physDevProperties.deviceName << std::endl;
            report << "    TYPE: " << resolvePhysicalDeviceTypeToString(physDevProperties.deviceType);
            report << "\n    ----\n";
            if (physDevProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
                gpuList.push_back(i);
            }
            if (physDevProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU) {
                integratedGpuList.push_back(i);
            }
        }
        gpuList.insert(gpuList.end(), integratedGpuList.begin(), integratedGpuList.end());
        int gpuIndex;
        for (gpuIndex = 0; gpuIndex < (int) gpuList.size() && physDevice == VK_NULL_HANDLE; ++gpuIndex) {
            physDevice = physDeviceList[gpuList[gpuIndex]];
            vkGetPhysicalDeviceProperties(physDevice, &physDevProperties);
            uint32_t familyCount;
            vkGetPhysicalDeviceQueueFamilyProperties(physDevice, &familyCount, NULL);
            if (familyCount < 1) {
                report << "No queue families found on " << physDevProperties.deviceName << std::endl;
                physDevice = VK_NULL_HANDLE;
                continue;
            }
            std::vector<VkQueueFamilyProperties> queueFamilyList(familyCount);
            vkGetPhysicalDeviceQueueFamilyProperties(physDevice, &familyCount, queueFamilyList.data());
            for (graphicsFamilyIndex = 0; graphicsFamilyIndex < familyCount; ++graphicsFamilyIndex) {
                if (queueFamilyList[graphicsFamilyIndex].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                    break;
                }
            }
            if (graphicsFamilyIndex >= familyCount) {
                report << "Graphics queue family not found on " << physDevProperties.deviceName << std::endl;
                physDevice = VK_NULL_HANDLE;
                continue;
            }
        }
        if (physDevice == VK_NULL_HANDLE) {
            std::cout << report.str();
            return VKBP_MSG("No appropriate GPU found.");
        }
        report << "Choosing to use " << physDevProperties.deviceName << std::endl;
        std::cout << report.str();

        //endregion
        //region CreateDevice

        float queuePriorities[]{0.f};
        VkDeviceQueueCreateInfo deviceQueueCreateInfo{};
        deviceQueueCreateInfo.sType             =   VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO ;
        deviceQueueCreateInfo.queueFamilyIndex  =                          graphicsFamilyIndex ;
        deviceQueueCreateInfo.queueCount        =                                            1 ;
        deviceQueueCreateInfo.pQueuePriorities  =                              queuePriorities ;

        VkDeviceCreateInfo deviceCreateInfo{};
        deviceCreateInfo.sType                      =           VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO ;
        deviceCreateInfo.queueCreateInfoCount       =                                              1 ;
        deviceCreateInfo.pQueueCreateInfos          =                         &deviceQueueCreateInfo ;
        deviceCreateInfo.enabledExtensionCount      =             (uint32_t) deviceExtensions.size() ;
        deviceCreateInfo.ppEnabledExtensionNames    =                        deviceExtensions.data() ;
        deviceCreateInfo.enabledLayerCount          =           instanceCreateInfo.enabledLayerCount ;
        deviceCreateInfo.ppEnabledLayerNames        =         instanceCreateInfo.ppEnabledLayerNames ;

        result = vkCreateDevice(physDevice, &deviceCreateInfo, nullptr, &logiDevice);
        VKBP_CHECK_ERR(result);
        vkGetDeviceQueue(logiDevice, graphicsFamilyIndex, 0, &queue);

        //endregion
        //region InitWindow

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindow = glfwCreateWindow(surfaceSizeX, surfaceSizeY, windowName.c_str(), nullptr, nullptr);
        glfwGetFramebufferSize(glfwWindow, (int*) &surfaceSizeX, (int*) &surfaceSizeY);

        //endregion
        //region InitSurface

        result = glfwCreateWindowSurface(instance, glfwWindow, nullptr, &surface);
        VKBP_CHECK_ERR(result);

        VkBool32 WSI_supported = (VkBool32) false;
        vkGetPhysicalDeviceSurfaceSupportKHR(physDevice, graphicsFamilyIndex, surface, &WSI_supported);
        if (!WSI_supported) {
            return VKBP_MSG("WSI is not supported!");
        }

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physDevice, surface, &surfaceCapabilities);
        if (surfaceCapabilities.currentExtent.width < UINT32_MAX) {
            surfaceSizeX = surfaceCapabilities.currentExtent.width;
            surfaceSizeY = surfaceCapabilities.currentExtent.height;
        }

        uint32_t formatCount = 0;
        vkGetPhysicalDeviceSurfaceFormatsKHR(physDevice, surface, &formatCount, nullptr);
        if (formatCount == 0) {
            return VKBP_MSG("Surface formats missing.");
        }
        // TODO: Don't just use the first surface format.
        std::vector<VkSurfaceFormatKHR> formats(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(physDevice, surface, &formatCount, formats.data());
        if (formats[0].format == VK_FORMAT_UNDEFINED) {
            surfaceFormat.format        =           VK_FORMAT_B8G8R8A8_UNORM ;
            surfaceFormat.colorSpace    =   VK_COLORSPACE_SRGB_NONLINEAR_KHR ;
        } else {
            surfaceFormat = formats[0];
        }

        //endregion
        //region InitSwapchain

        if (swapchainImageCount < surfaceCapabilities.minImageCount + 1) {
            swapchainImageCount = surfaceCapabilities.minImageCount + 1;
        }
        if (surfaceCapabilities.maxImageCount > 0) {
            if (swapchainImageCount > surfaceCapabilities.maxImageCount) {
                swapchainImageCount = surfaceCapabilities.maxImageCount;
            }
        }

        //TODO: support modes other than FIFO and MAILBOX
        uint32_t presentModeCount = 0;
        result = vkGetPhysicalDeviceSurfacePresentModesKHR(physDevice, surface, &presentModeCount, nullptr);
        VKBP_CHECK_ERR(result);
        std::vector<VkPresentModeKHR> presentModeList(presentModeCount);
        result = vkGetPhysicalDeviceSurfacePresentModesKHR(physDevice, surface, &presentModeCount,
                                                           presentModeList.data());
        VKBP_CHECK_ERR(result);
        VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;
        for (auto mode : presentModeList) {
            if (mode == VK_PRESENT_MODE_MAILBOX_KHR) {
                presentMode = mode;
            }
        }

        VkSwapchainCreateInfoKHR swapchainCreateInfo{};
        swapchainCreateInfo.sType                   = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR ;
        swapchainCreateInfo.surface                 =                                     surface ;
        swapchainCreateInfo.minImageCount           =                         swapchainImageCount ;
        swapchainCreateInfo.imageFormat             =                        surfaceFormat.format ;
        swapchainCreateInfo.imageColorSpace         =                    surfaceFormat.colorSpace ;
        swapchainCreateInfo.imageExtent.width       =                                surfaceSizeX ;
        swapchainCreateInfo.imageExtent.height      =                                surfaceSizeY ;
        swapchainCreateInfo.imageArrayLayers        =                                           1 ;
        swapchainCreateInfo.imageUsage              =         VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT ;
        swapchainCreateInfo.imageSharingMode        =                   VK_SHARING_MODE_EXCLUSIVE ;
        swapchainCreateInfo.queueFamilyIndexCount   =                                           0 ;
        swapchainCreateInfo.pQueueFamilyIndices     =                                     nullptr ;
        swapchainCreateInfo.preTransform            =       VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR ;
        swapchainCreateInfo.compositeAlpha          =           VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR ;
        swapchainCreateInfo.presentMode             =                                 presentMode ;
        swapchainCreateInfo.clipped                 =                                     VK_TRUE ;
        swapchainCreateInfo.oldSwapchain            =                              VK_NULL_HANDLE ;

        result = vkCreateSwapchainKHR(logiDevice, &swapchainCreateInfo, nullptr, &swapchain);
        VKBP_CHECK_ERR(result);

        result = vkGetSwapchainImagesKHR(logiDevice, swapchain, &swapchainImageCount, nullptr);
        VKBP_CHECK_ERR(result);

        swapchainImages.resize(swapchainImageCount);
        swapchainImageViews.resize(swapchainImageCount);

        result = vkGetSwapchainImagesKHR(logiDevice, swapchain, &swapchainImageCount, swapchainImages.data());
        VKBP_CHECK_ERR(result);

        for (uint32_t i = 0; i < swapchainImageCount; ++i) {
            VkImageViewCreateInfo image_view_create_info{};
            image_view_create_info.sType                            = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO ;
            image_view_create_info.image                            =                       swapchainImages[i] ;
            image_view_create_info.viewType                         =                    VK_IMAGE_VIEW_TYPE_2D ;
            image_view_create_info.format                           =                     surfaceFormat.format ;
            image_view_create_info.components.r                     =            VK_COMPONENT_SWIZZLE_IDENTITY ;
            image_view_create_info.components.g                     =            VK_COMPONENT_SWIZZLE_IDENTITY ;
            image_view_create_info.components.b                     =            VK_COMPONENT_SWIZZLE_IDENTITY ;
            image_view_create_info.components.a                     =            VK_COMPONENT_SWIZZLE_IDENTITY ;
            image_view_create_info.subresourceRange.aspectMask      =                VK_IMAGE_ASPECT_COLOR_BIT ;
            image_view_create_info.subresourceRange.baseMipLevel    =                                        0 ;
            image_view_create_info.subresourceRange.levelCount      =                                        1 ;
            image_view_create_info.subresourceRange.baseArrayLayer  =                                        0 ;
            image_view_create_info.subresourceRange.layerCount      =                                        1 ;

            result = vkCreateImageView(logiDevice, &image_view_create_info, nullptr, &swapchainImageViews[i]);
            VKBP_CHECK_ERR_MSG(result, std::string("(iteration " + std::to_string(i)).c_str());
        }

        //endregion

        //region mainloop

        bool shouldRun = true;
        while (shouldRun) {
            if (glfwWindow) {
                glfwPollEvents();
                if (glfwWindowShouldClose(glfwWindow)) {
                    shouldRun = false;
                    std::cout << "Window received exit event.\n";
                }
            }
        }

        //endregion

        //region Destruct

        for (auto view : swapchainImageViews) {
            vkDestroyImageView(logiDevice, view, nullptr);
        }
        vkDestroySwapchainKHR(logiDevice, swapchain, nullptr);

        vkDestroySurfaceKHR(instance, surface, nullptr);

        glfwDestroyWindow(glfwWindow);

        vkDestroyDevice(logiDevice, nullptr);
        logiDevice = nullptr;

        res = validator.destroyDebugReportCallbackBeforeInstanceDestruction(instance);
        if (res.isError()) { returnValue = res; }

        vkDestroyInstance(instance, nullptr);
        instance = nullptr;

        glfwTerminate();

        //endregion

        return returnValue;
    }
}
