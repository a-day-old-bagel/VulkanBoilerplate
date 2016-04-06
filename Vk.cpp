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
#include "vulkan/vulkan.h"
//#include "vkbpGlobal.h"
#include "Vk.h"
#include "vulkanHelpers.h"

#ifdef WIN32
#include <cstdio>
#endif

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

    VkBool32 messageCallback(
            VkDebugReportFlagsEXT flags,
            VkDebugReportObjectTypeEXT objType,
            uint64_t srcObject,
            size_t location,
            int32_t msgCode,
            const char* pLayerPrefix,
            const char* pMsg,
            void* pUserData) {
        char *message = (char *)malloc(std::strlen(pMsg) + 100);
        if (!message) {
            std::cout << "BAD MESSAGE at line " << __LINE__ << std::endl;
            exit(1);
        }
        if (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT) {
            std::cout << "ERROR: " << "[" << pLayerPrefix << "] Code " << msgCode << " : " << pMsg << "\n";
        } else if (flags & VK_DEBUG_REPORT_WARNING_BIT_EXT) {
            std::cout << "WARNING: " << "[" << pLayerPrefix << "] Code " << msgCode << " : " << pMsg << "\n";
        } else {
            return (VkBool32)false;
        }
        fflush(stdout);
        free(message);
        return (VkBool32)false;
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
            "VK_LAYER_GOOGLE_threading",
            "VK_LAYER_LUNARG_mem_tracker",
            "VK_LAYER_LUNARG_object_tracker",
            "VK_LAYER_LUNARG_draw_state",
            "VK_LAYER_LUNARG_param_checker",
            "VK_LAYER_LUNARG_swapchain",
            "VK_LAYER_LUNARG_device_limits",
            "VK_LAYER_LUNARG_image",
            "VK_LAYER_GOOGLE_unique_objects",
        };

#ifdef WIN32
        AllocConsole();
        AttachConsole(GetCurrentProcessId());
        freopen("CON", "w", stdout);
        SetConsoleTitle(TEXT(title.c_str()));
        if (enableValidation) {
            std::cout << "Validation enabled:\n";
        }
#else
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
#endif

        VkApplicationInfo appInfo = {};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = name.c_str();
        appInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 1);
        appInfo.pEngineName = name.c_str();
        appInfo.engineVersion = VK_MAKE_VERSION(0, 0, 1);
        appInfo.apiVersion = VK_MAKE_VERSION(1, 0, 0);
        std::vector<const char*> enabledExtensions;
        enabledExtensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
        enabledExtensions.push_back(Settings::windowing_system_extension_name);

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

        uint32_t physicalDeviceCount = 0;
        ret = vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, NULL);
        if (ret != VK_SUCCESS) {
            printf("Unable to enumerate physical devices!\n");
            return ret;
        }
        if (physicalDeviceCount <= 0) {
            printf("No GPU found!");
            exit(1);
        }
        std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
        ret = vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDevices.data());
        if (ret != VK_SUCCESS) {
            printf("Unable to enumerate physical devices!\n");
            return ret;
        }

        int whichPhysicalDevice = 0;
        VkPhysicalDeviceType currentPhysicalDeviceType = VK_PHYSICAL_DEVICE_TYPE_OTHER;
        std::cout << physicalDeviceCount << " Physical devices recognized:\n";
        VkPhysicalDeviceProperties deviceProperties = {};
        for (uint32_t i = 0; i < physicalDeviceCount; ++i) {
            vkGetPhysicalDeviceProperties(physicalDevices[i], &deviceProperties);
            std::cout << "    NAME: " << deviceProperties.deviceName << std::endl;
            std::cout << "    TYPE: " << resolvePhysicalDeviceTypeToString(deviceProperties.deviceType);
            std::cout << std::endl;
            if ((currentPhysicalDeviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
                 currentPhysicalDeviceType != VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU &&
                 deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU) ||
                (currentPhysicalDeviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
                 deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)) {
                whichPhysicalDevice = i;
                currentPhysicalDeviceType = deviceProperties.deviceType;
            }
        }
        vkGetPhysicalDeviceProperties(physicalDevices[whichPhysicalDevice], &deviceProperties);
        std::cout << "Choosing to use: " << deviceProperties.deviceName << std::endl;
        physicalDevice = physicalDevices[whichPhysicalDevice];

        uint32_t graphicsQueueIndex;
        uint32_t graphicsQueueCount;
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &graphicsQueueCount, NULL);
        if (graphicsQueueCount < 1) {
            printf("Queue family properties not found!\n");
            exit(1);
        }
        std::vector<VkQueueFamilyProperties> graphicsQueueProps(graphicsQueueCount);
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &graphicsQueueCount, graphicsQueueProps.data());
        for (graphicsQueueIndex = 0; graphicsQueueIndex < graphicsQueueCount; ++graphicsQueueIndex) {
            if (graphicsQueueProps[graphicsQueueIndex].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                break;
            }
        }
        if (graphicsQueueIndex >= graphicsQueueCount) {
            printf("Graphics queue not found!\n");
            exit(1);
        }

        std::vector<float> queuePriorities;
        queuePriorities.push_back(0.f);
        VkDeviceQueueCreateInfo queueCreateInfo = {};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = graphicsQueueIndex;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = queuePriorities.data();
        std::vector<const char*> enabledDeviceExtensions;
        enabledDeviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
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
        std::vector<VkFormat> depthFormats;
        depthFormats.push_back(VK_FORMAT_D32_SFLOAT_S8_UINT);
        depthFormats.push_back(VK_FORMAT_D32_SFLOAT);
        depthFormats.push_back(VK_FORMAT_D24_UNORM_S8_UINT);
        depthFormats.push_back(VK_FORMAT_D16_UNORM_S8_UINT);
        depthFormats.push_back(VK_FORMAT_D16_UNORM);
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

        #define GET_INSTANCE_PROC_ADDR(inst, entrypoint)                        \
        {                                                                       \
            fp##entrypoint = (PFN_vk##entrypoint) vkGetInstanceProcAddr(inst, "vk"#entrypoint); \
            if (fp##entrypoint == NULL)                                         \
            {																    \
                exit(1);                                                        \
            }                                                                   \
        }
        #define GET_DEVICE_PROC_ADDR(dev, entrypoint)                           \
        {                                                                       \
            fp##entrypoint = (PFN_vk##entrypoint) vkGetDeviceProcAddr(dev, "vk"#entrypoint);   \
            if (fp##entrypoint == NULL)                                         \
            {																    \
                exit(1);                                                        \
            }                                                                   \
        }
        GET_INSTANCE_PROC_ADDR(instance, GetPhysicalDeviceSurfaceSupportKHR);
        GET_INSTANCE_PROC_ADDR(instance, GetPhysicalDeviceSurfaceCapabilitiesKHR);
        GET_INSTANCE_PROC_ADDR(instance, GetPhysicalDeviceSurfaceFormatsKHR);
        GET_INSTANCE_PROC_ADDR(instance, GetPhysicalDeviceSurfacePresentModesKHR);
        GET_DEVICE_PROC_ADDR(device, CreateSwapchainKHR);
        GET_DEVICE_PROC_ADDR(device, DestroySwapchainKHR);
        GET_DEVICE_PROC_ADDR(device, GetSwapchainImagesKHR);
        GET_DEVICE_PROC_ADDR(device, AcquireNextImageKHR);
        GET_DEVICE_PROC_ADDR(device, QueuePresentKHR);
        #undef GET_INSTANCE_PROC_ADDR
        #undef GET_DEVICE_PROC_ADDR

        VkSemaphoreCreateInfo semaphoreCreateInfo = {};
        semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        semaphoreCreateInfo.pNext = NULL;
        semaphoreCreateInfo.flags = 0;
        struct {
            VkSemaphore presentComplete;
            VkSemaphore renderComplete;
        } semaphores;
        ret = vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &semaphores.presentComplete);
        if (ret != VK_SUCCESS) {
            printf("Could not create Vulkan semaphore (presentComplete)!\n");
            exit(1);
        }
        ret = vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &semaphores.renderComplete);
        if (ret != VK_SUCCESS) {
            printf("Could not create Vulkan semaphore (renderComplete)!\n");
            exit(1);
        }

        VkPipelineStageFlags submitPipelineStages = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.pNext = NULL;
        submitInfo.pWaitDstStageMask = &submitPipelineStages;
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = &semaphores.presentComplete;
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = &semaphores.renderComplete;



#ifdef WIN32
#else
        uint32_t value_mask, value_list[32];
        window = xcb_generate_id(connection);
        value_mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
        value_list[0] = screen->black_pixel;
        value_list[1] =
                XCB_EVENT_MASK_KEY_RELEASE |
                XCB_EVENT_MASK_EXPOSURE |
                XCB_EVENT_MASK_STRUCTURE_NOTIFY |
                XCB_EVENT_MASK_POINTER_MOTION |
                XCB_EVENT_MASK_BUTTON_PRESS |
                XCB_EVENT_MASK_BUTTON_RELEASE;
        xcb_create_window(connection,
                          XCB_COPY_FROM_PARENT,
                          window, screen->root,
                          0, 0, width, height, 0,
                          XCB_WINDOW_CLASS_INPUT_OUTPUT,
                          screen->root_visual,
                          value_mask, value_list);
        xcb_intern_atom_cookie_t cookie = xcb_intern_atom(connection, 1, 12, "WM_PROTOCOLS");
        xcb_intern_atom_reply_t* reply = xcb_intern_atom_reply(connection, cookie, 0);
        xcb_intern_atom_cookie_t cookie2 = xcb_intern_atom(connection, 0, 16, "WM_DELETE_WINDOW");
        atom_wm_delete_window = xcb_intern_atom_reply(connection, cookie2, 0);
        xcb_change_property(connection, XCB_PROP_MODE_REPLACE,
                            window, (*reply).atom, 4, 32, 1,
                            &(*atom_wm_delete_window).atom);
        xcb_change_property(connection, XCB_PROP_MODE_REPLACE,
                            window, XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8,
                            title.size(), title.c_str());
        free(reply);
        xcb_map_window(connection, window);



        VkXcbSurfaceCreateInfoKHR surfaceCreateInfo = {};
        surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
        surfaceCreateInfo.connection = connection;
        surfaceCreateInfo.window = window;
        ret = vkCreateXcbSurfaceKHR(instance, &surfaceCreateInfo, NULL, &surface);
#endif



        uint32_t queueCount;
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueCount, NULL);
        if (queueCount <= 0) {
            printf("Could not get physical device queue family property count!\n");
            exit(1);
        }
        std::vector<VkQueueFamilyProperties> queueProps(queueCount);
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueCount, queueProps.data());
        std::vector<VkBool32> supportsPresent(queueCount);
        for (uint32_t i = 0; i < queueCount; i++) {
            fpGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &supportsPresent[i]);
        }
        uint32_t graphicsQueueNodeIndex = UINT32_MAX;
        uint32_t presentQueueNodeIndex = UINT32_MAX;
        for (uint32_t i = 0; i < queueCount; i++) {
            if ((queueProps[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0) {
                if (graphicsQueueNodeIndex == UINT32_MAX) {
                    graphicsQueueNodeIndex = i;
                }
                if (supportsPresent[i] == VK_TRUE) {
                    graphicsQueueNodeIndex = i;
                    presentQueueNodeIndex = i;
                    break;
                }
            }
        }
        if (presentQueueNodeIndex == UINT32_MAX) {
            for (uint32_t i = 0; i < queueCount; ++i) {
                if (supportsPresent[i] == VK_TRUE) {
                    presentQueueNodeIndex = i;
                    break;
                }
            }
        }
        if (graphicsQueueNodeIndex == UINT32_MAX || presentQueueNodeIndex == UINT32_MAX) {
            printf("Could not find a graphics and/or presenting queue!");
            exit(1);
        }
        // TODO: use different graphics and presenting queues
        if (graphicsQueueNodeIndex != presentQueueNodeIndex) {
            printf("Separate graphics and presenting queues are not supported yet!");
            exit(1);
        }
//        uint32_t queueNodeIndex = UINT32_MAX;
//        queueNodeIndex = graphicsQueueNodeIndex;
        uint32_t formatCount;
        ret = fpGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, NULL);
        if (ret != VK_SUCCESS || formatCount <= 0) {
            printf("Could not get physical device surface format count!\n");
            exit(1);
        }
        std::vector<VkSurfaceFormatKHR> surfaceFormats(formatCount);
        ret = fpGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, surfaceFormats.data());
        if (ret != VK_SUCCESS) {
            printf("Could not get physical device surface format!\n");
            exit(1);
        }
        VkFormat colorFormat;
        if ((formatCount == 1) && (surfaceFormats[0].format == VK_FORMAT_UNDEFINED)) {
            colorFormat = VK_FORMAT_B8G8R8A8_UNORM;
        } else {
            // TODO: don't just select the first one
            colorFormat = surfaceFormats[0].format;
        }
        VkColorSpaceKHR colorSpace;
        colorSpace = surfaceFormats[0].colorSpace;



        if (enableValidation) {
            CreateDebugReportCallback = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT");
            DestroyDebugReportCallback = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT");
            dbgBreakCallback = (PFN_vkDebugReportMessageEXT)vkGetInstanceProcAddr(instance, "vkDebugReportMessageEXT");

            VkDebugReportCallbackCreateInfoEXT dbgCreateInfo;
            dbgCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
            dbgCreateInfo.pNext = NULL;
            dbgCreateInfo.pfnCallback = (PFN_vkDebugReportCallbackEXT) messageCallback;
            dbgCreateInfo.pUserData = NULL;
            dbgCreateInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
            VkDebugReportCallbackEXT debugReportCallback;
            ret = CreateDebugReportCallback(
                    instance,
                    &dbgCreateInfo,
                    NULL,
                    &debugReportCallback);
            if (ret != VK_SUCCESS) {
                printf("Could not create debug report callback!\n");
                exit(1);
            }
        }




        VkCommandPool cmdPool;
        uint32_t queueNodeIndex = UINT32_MAX;
        VkCommandPoolCreateInfo cmdPoolInfo = {};
        cmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        cmdPoolInfo.queueFamilyIndex = queueNodeIndex;
        cmdPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        ret = vkCreateCommandPool(device, &cmdPoolInfo, nullptr, &cmdPool);
        if (ret != VK_SUCCESS) {
            printf("Could not create command pool!\n");
            exit(1);
        }


        std::getchar();
        return VK_SUCCESS;
    }
}
