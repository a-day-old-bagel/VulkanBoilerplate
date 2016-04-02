/**
 * part of VKBP (Vulkan Boilerplate)
 *
 * Galen Cochrane
 * Copyright (C) 2016
 * This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
 */

#include <array>
#include "vulkan/vulkan.h"
#include "Vk.h"
#include "vulkanHelpers.h"

namespace vkbp {

    Vk::Vk() {
        instance = 0;
        hasInit = false;
    }

    Vk::~Vk() {
        if (hasInit) {
            vkDestroyInstance(instance, NULL);
        }
    }

    VkResult Vk::init(const char* appName, const char* engName) {

        VkApplicationInfo appInfo;
        FillVkApplicationInfo(appName, engName, appInfo);

        std::vector<const char*> vkExtensionsToUse;
        AppendBasicVkExtensions(vkExtensionsToUse);

        VkInstanceCreateInfo instanceInfo;
        FillVkInstanceInfo(appInfo, vkExtensionsToUse, instanceInfo);

        VkResult res = vkCreateInstance(&instanceInfo, NULL, &instance);
        if (res == VK_SUCCESS) {
            hasInit = true;
        }
        return res;
    }

    VkResult Vk::initSimple() {
        int width = 800;
        int height = 600;
        float zoom = -2.f;
        std::string title = "Basic Vulkan";
        std::string name = "Basic Vulkan";
        bool enableValidation = true;
        uint32_t validationLayerCount = 9;
        const char *validationLayerNames[] = {
            "VK_LAYER_LUNARG_threading",
            "VK_LAYER_LUNARG_mem_tracker",
            "VK_LAYER_LUNARG_object_tracker",
            "VK_LAYER_LUNARG_draw_state",
            "VK_LAYER_LUNARG_param_checker",
            "VK_LAYER_LUNARG_swapchain",
            "VK_LAYER_LUNARG_device_limits",
            "VK_LAYER_LUNARG_image",
            "VK_LAYER_GOOGLE_unique_objects",
        };

        const xcb_setup_t* setup;
        xcb_screen_iterator_t iter;
        int scr;
        xcb_connection_t *connection;
        xcb_screen_t *screen;
        xcb_window_t window;
        xcb_intern_atom_reply_t *atom_wm_delete_window;

        connection = xcb_connect(NULL, &scr);
        if (connection == NULL) {
            printf("Could not find a compatible Vulkan ICD!\n");
            exit(1);
        }
        setup = xcb_get_setup(connection);
        iter = xcb_setup_roots_iterator(setup);
        while(scr-- > 0) {
            xcb_screen_next(&iter);
        }
        screen = iter.data;

        VkApplicationInfo appInfo = {};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = name.c_str();
        appInfo.pEngineName = name.c_str();
        appInfo.apiVersion = VK_API_VERSION_1_0;
        std::vector<const char*> enabledExtensions = {
            VK_KHR_SURFACE_EXTENSION_NAME,
            VK_KHR_XCB_SURFACE_EXTENSION_NAME
        };

        VkInstanceCreateInfo instanceCreateInfo = {};
        instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instanceCreateInfo.pNext = NULL;
        instanceCreateInfo.pApplicationInfo = &appInfo;
        if (enabledExtensions.size()) {
            if (enableValidation) {
                enabledExtensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
            }
            instanceCreateInfo.enabledExtensionCount = (uint32_t)enabledExtensions.size();
            instanceCreateInfo.ppEnabledExtensionNames = enabledExtensions.data();
        } else {
            instanceCreateInfo.enabledExtensionCount = 0;
            instanceCreateInfo.ppEnabledExtensionNames = NULL;
        }
        if (enableValidation) {
            instanceCreateInfo.enabledLayerCount = validationLayerCount;
            instanceCreateInfo.ppEnabledLayerNames = validationLayerNames;
        } else {
            instanceCreateInfo.enabledLayerCount = 0;
            instanceCreateInfo.ppEnabledLayerNames = NULL;
        }

        VkResult ret;
        ret = vkCreateInstance(&instanceCreateInfo, NULL, &instance);
        if (ret != VK_SUCCESS) {
            printf("Vulkan instance not created!\n");
            return ret;
        }

        uint32_t gpuCount = 0;
        ret = vkEnumeratePhysicalDevices(instance, &gpuCount, NULL);
        if (ret != VK_SUCCESS) {
            printf("Unable to enumerate physical devices!\n");
            return ret;
        }
        if (gpuCount <= 0) {
            printf("No GPU found!");
            exit(1);
        }
        std::vector<VkPhysicalDevice> physicalDevices(gpuCount);
        ret = vkEnumeratePhysicalDevices(instance, &gpuCount, physicalDevices.data());
        if (ret != VK_SUCCESS) {
            printf("Unable to enumerate physical devices!\n");
            return ret;
        }

        //TODO: CHANGE THIS NOW
        physicalDevice = physicalDevices[0];

        uint32_t graphicsQueueIndex;
        uint32_t queueCount;
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueCount, NULL);
        if (queueCount < 1) {
            printf("Queue family properties not found!\n");
            exit(1);
        }
        std::vector<VkQueueFamilyProperties> queueProps(queueCount);
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueCount, queueProps.data());
        for (graphicsQueueIndex = 0; graphicsQueueIndex < queueCount; ++graphicsQueueIndex) {
            if (queueProps[graphicsQueueIndex].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                break;
            }
        }
        if (graphicsQueueIndex >= queueCount) {
            printf("Graphics queue not found!\n");
            exit(1);
        }

        std::array<float, 1> queuePriorities = {0.f};
        VkDeviceQueueCreateInfo queueCreateInfo = {};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = graphicsQueueIndex;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = queuePriorities.data();
        std::vector<const char*> enabledDeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
        VkDeviceCreateInfo deviceCreateInfo = {};
        deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        deviceCreateInfo.pNext = NULL;
        deviceCreateInfo.queueCreateInfoCount = 1;
        deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
        deviceCreateInfo.pEnabledFeatures = NULL;
        if (enabledDeviceExtensions.size()) {
            deviceCreateInfo.enabledExtensionCount = (uint32_t)enabledDeviceExtensions.size();
            deviceCreateInfo.ppEnabledExtensionNames = enabledDeviceExtensions.data();
        } else {
            deviceCreateInfo.enabledExtensionCount = 0;
            deviceCreateInfo.ppEnabledExtensionNames = NULL;
        }
        if (enableValidation) {
            deviceCreateInfo.enabledLayerCount = validationLayerCount;
            deviceCreateInfo.ppEnabledLayerNames = validationLayerNames;
        } else {
            deviceCreateInfo.enabledLayerCount = 0;
            deviceCreateInfo.ppEnabledLayerNames = NULL;
        }
        ret = vkCreateDevice(physicalDevice, &deviceCreateInfo, NULL, &device);
        if (ret != VK_SUCCESS) {
            printf("Could not create logical device");
            return ret;
        }

        VkPhysicalDeviceMemoryProperties deviceMemoryProperties;
        vkGetPhysicalDeviceMemoryProperties(physicalDevice, &deviceMemoryProperties);
        vkGetDeviceQueue(device, graphicsQueueIndex, 0, &queue);

        VkFormat depthFormat = VK_FORMAT_UNDEFINED;
        std::vector<VkFormat> depthFormats = {
                VK_FORMAT_D32_SFLOAT_S8_UINT,
                VK_FORMAT_D32_SFLOAT,
                VK_FORMAT_D24_UNORM_S8_UINT,
                VK_FORMAT_D16_UNORM_S8_UINT,
                VK_FORMAT_D16_UNORM
        };
        for (auto format : depthFormats) {
            VkFormatProperties formatProps;
            vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &formatProps);
            if (formatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
                depthFormat = format;
            }
        }
        if (depthFormat == VK_FORMAT_UNDEFINED) {
            printf("Could not determine depth format!\n");
            exit(1);
        }

        // TODO look at swapChain.connect(instance, physicalDevice, device) at vulkanExampleBase.cpp:578
    }
}
