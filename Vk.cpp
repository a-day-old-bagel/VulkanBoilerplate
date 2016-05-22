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
#include "vkbpHelpers.h"

#ifdef WIN32
#include <cstdio>
#endif

namespace vkbp {

    Vk::Vk() : instance(0), hasInit(false), setupCmdBuffer(VK_NULL_HANDLE), swapChain(VK_NULL_HANDLE),
               width(1280), height(720), imageCount(0) {

    }

    Vk::~Vk() {
        if (hasInit) {
            vkDestroyInstance(instance, NULL);
        }
    }

    VkbpResult Vk::init(const char* appName, const char* engName) {

        VkApplicationInfo appInfo;
        FillVkApplicationInfo(appName, engName, appInfo);

        std::vector<const char*> vkExtensionsToUse;
        AppendBasicVkExtensions(vkExtensionsToUse);

        VkInstanceCreateInfo instanceInfo;
        FillVkInstanceInfo(appInfo, vkExtensionsToUse, instanceInfo);

        VkResult res = vkCreateInstance(&instanceInfo, NULL, &instance);
        if (res != VK_SUCCESS) {
            return VKBP_ERR(res);
        }

        hasInit = true;
        return VKBP_SUCCESS(nullptr);
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

    VkbpResult Vk::initSimple() {
        float zoom = -2.f;
        std::string title = "Basic Vulkan";
        std::string name = "Basic Vulkan";
        bool enableValidation = true;      //TODO:this is crap
        uint32_t validationLayerCount = 1;
        const char *validationLayerNames[] = {
            "VK_LAYER_LUNARG_standard_validation"   // includes a bunch of commonly-used layers
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
            return VKBP_MSG("Could not find a compatible Vulkan ICD!");
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
            return VKBP_ERR_MSG(ret, "Vulkan instance not created!");
        }

        uint32_t physicalDeviceCount = 0;
        ret = vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, NULL);
        if (ret != VK_SUCCESS) {
            return VKBP_ERR_MSG(ret, "Unable to enumerate physical devices!");
        }
        if (physicalDeviceCount <= 0) {
            return VKBP_MSG("No GPU found!");
        }
        std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
        ret = vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDevices.data());
        if (ret != VK_SUCCESS) {
            return VKBP_ERR_MSG(ret, "Unable to enumerate physical devices!");
        }

        int whichPhysicalDevice = 0;
        VkPhysicalDeviceType currentPhysicalDeviceType = VK_PHYSICAL_DEVICE_TYPE_OTHER;
        std::cout << "Physical devices recognized: (" << physicalDeviceCount << " total): \n    ----\n";
        VkPhysicalDeviceProperties deviceProperties = {};
        for (uint32_t i = 0; i < physicalDeviceCount; ++i) {
            vkGetPhysicalDeviceProperties(physicalDevices[i], &deviceProperties);
            std::cout << "    NAME: " << deviceProperties.deviceName << std::endl;
            std::cout << "    TYPE: " << resolvePhysicalDeviceTypeToString(deviceProperties.deviceType);
            std::cout << "\n    ----\n";
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
            return VKBP_MSG("Queue family properties not found!");
        }
        std::vector<VkQueueFamilyProperties> graphicsQueueProps(graphicsQueueCount);
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &graphicsQueueCount, graphicsQueueProps.data());
        for (graphicsQueueIndex = 0; graphicsQueueIndex < graphicsQueueCount; ++graphicsQueueIndex) {
            if (graphicsQueueProps[graphicsQueueIndex].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                break;
            }
        }
        if (graphicsQueueIndex >= graphicsQueueCount) {
            return VKBP_MSG("Graphics queue not found!");
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
            return VKBP_ERR_MSG(ret, "Could not create logical device!");
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
            return VKBP_MSG("Could not determine depth format!");
        }

        #define GET_INSTANCE_PROC_ADDR(inst, entrypoint)                        \
        {                                                                       \
            fp##entrypoint = (PFN_vk##entrypoint) vkGetInstanceProcAddr(inst, "vk"#entrypoint); \
            if (fp##entrypoint == NULL)                                         \
            {																    \
                return VKBP_MSG("Could not get address for "#entrypoint);       \
            }                                                                   \
        }
        #define GET_DEVICE_PROC_ADDR(dev, entrypoint)                           \
        {                                                                       \
            fp##entrypoint = (PFN_vk##entrypoint) vkGetDeviceProcAddr(dev, "vk"#entrypoint);   \
            if (fp##entrypoint == NULL)                                         \
            {																    \
                return VKBP_MSG("Could not get address for "#entrypoint);       \
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
            return VKBP_ERR_MSG(ret, "Could not create Vulkan semaphore (presentComplete)!");
        }
        ret = vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &semaphores.renderComplete);
        if (ret != VK_SUCCESS) {
            return VKBP_ERR_MSG(ret, "Could not create Vulkan semaphore (renderComplete)!");
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
            return VKBP_MSG("Could not get physical device queue family property count!");
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
            return VKBP_MSG("Could not find a graphics and/or presenting queue!");
        }
        // TODO: use different graphics and presenting queues
        if (graphicsQueueNodeIndex != presentQueueNodeIndex) {
            return VKBP_MSG("Separate graphics and presenting queues are not supported!");
        }

        uint32_t formatCount;
        ret = fpGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, NULL);
        if (ret != VK_SUCCESS || formatCount <= 0) {
            return VKBP_ERR_MSG(ret, "Could not get physical device surface format count!");
        }
        std::vector<VkSurfaceFormatKHR> surfaceFormats(formatCount);
        ret = fpGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, surfaceFormats.data());
        if (ret != VK_SUCCESS) {
            return VKBP_ERR_MSG(ret, "Could not get physical device surface format!");
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

        //region setupDebugging

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
                return VKBP_ERR_MSG(ret, "Could not create debug report callback!");
            }
        }

        //endregion

        //region createCommandPool

        VkCommandPool cmdPool;
        VkCommandPoolCreateInfo cmdPoolInfo = {};
        cmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        cmdPoolInfo.queueFamilyIndex = graphicsQueueNodeIndex;  // todo : don't know if this is right to use
        cmdPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        ret = vkCreateCommandPool(device, &cmdPoolInfo, nullptr, &cmdPool);
        if (ret != VK_SUCCESS) {
            return VKBP_ERR_MSG(ret, "Could not create command pool!");
        }

        //endregion

        //region createSetupCommandBuffer

        if (setupCmdBuffer != VK_NULL_HANDLE)
        {
            vkFreeCommandBuffers(device, cmdPool, 1, &setupCmdBuffer);
            setupCmdBuffer = VK_NULL_HANDLE; // todo : check if still necessary
        }

        VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
        commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        commandBufferAllocateInfo.commandPool = cmdPool;
        commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        commandBufferAllocateInfo.commandBufferCount = 1;

        ret = vkAllocateCommandBuffers(device, &commandBufferAllocateInfo, &setupCmdBuffer);
        if (ret != VK_SUCCESS) {
            return VKBP_ERR(ret);
        }

        VkCommandBufferBeginInfo cmdBufInfo = {};
        cmdBufInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        ret = vkBeginCommandBuffer(setupCmdBuffer, &cmdBufInfo);
        if (ret != VK_SUCCESS) {
            return VKBP_ERR(ret);
        }

        //endregion

        //region createSwapChain

        VkSwapchainKHR oldSwapchain = swapChain;

        VkSurfaceCapabilitiesKHR surfCaps;
        ret = fpGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfCaps);
        if (ret != VK_SUCCESS) {
            return VKBP_ERR(ret);
        }

        uint32_t presentModeCount;
        ret = fpGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, NULL);
        if (ret != VK_SUCCESS) {
            return VKBP_ERR(ret);
        }
        if (!presentModeCount) {
            return VKBP_MSG("Couldn't find any present modes!");
        }

        std::vector<VkPresentModeKHR> presentModes(presentModeCount);

        if (ret != VK_SUCCESS) {
            return VKBP_ERR(ret);
        }

        VkExtent2D swapchainExtent = {};
        if (surfCaps.currentExtent.width == -1) {   // -1 indicates undefined: use default values
            swapchainExtent.width = width;
            swapchainExtent.height = height;
        } else {                                    // otherwise 1 indicates self-defined (can only be -1 or 1).
            swapchainExtent = surfCaps.currentExtent;
            width = surfCaps.currentExtent.width;
            height = surfCaps.currentExtent.height;
        }

        VkPresentModeKHR swapchainPresentMode = VK_PRESENT_MODE_FIFO_KHR;   // Todo: This is ensured to work?
        for (size_t i = 0; i < presentModeCount; i++)
        {
            if (presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
                swapchainPresentMode = VK_PRESENT_MODE_MAILBOX_KHR; // This is preferred
                break;
            }
            if (presentModes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR) {
                swapchainPresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;   // This is okay too
            }
        }

        // Determine the number of images
        uint32_t desiredNumberOfSwapchainImages = surfCaps.minImageCount + 1;
        if ((surfCaps.maxImageCount > 0) && (desiredNumberOfSwapchainImages > surfCaps.maxImageCount))
        {
            desiredNumberOfSwapchainImages = surfCaps.maxImageCount;
        }

        VkSurfaceTransformFlagsKHR preTransform;
        if (surfCaps.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
        {
            preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
        }
        else
        {
            preTransform = surfCaps.currentTransform;
        }

        VkSwapchainCreateInfoKHR swapchainCI = {};
        swapchainCI.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapchainCI.pNext = NULL;
        swapchainCI.surface = surface;
        swapchainCI.minImageCount = desiredNumberOfSwapchainImages;
        swapchainCI.imageFormat = colorFormat;
        swapchainCI.imageColorSpace = colorSpace;
        swapchainCI.imageExtent = { swapchainExtent.width, swapchainExtent.height };
        swapchainCI.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        swapchainCI.preTransform = (VkSurfaceTransformFlagBitsKHR)preTransform;
        swapchainCI.imageArrayLayers = 1;
        swapchainCI.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapchainCI.queueFamilyIndexCount = 0;
        swapchainCI.pQueueFamilyIndices = NULL;
        swapchainCI.presentMode = swapchainPresentMode;
        swapchainCI.oldSwapchain = oldSwapchain;
        swapchainCI.clipped = (VkBool32)true;
        swapchainCI.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

        ret = fpCreateSwapchainKHR(device, &swapchainCI, nullptr, &swapChain);
        if (ret != VK_SUCCESS) {
            return VKBP_ERR(ret);
        }

        // clean up any existing swap chain
        if (oldSwapchain != VK_NULL_HANDLE)
        {
            for (uint32_t i = 0; i < imageCount; i++)
            {
                vkDestroyImageView(device, buffers[i].view, nullptr);
            }
            fpDestroySwapchainKHR(device, oldSwapchain, nullptr);
        }

        ret = fpGetSwapchainImagesKHR(device, swapChain, &imageCount, NULL);
        if (ret != VK_SUCCESS) {
            return VKBP_ERR(ret);
        }

        // make images for swap chains
        images.resize(imageCount);
        ret = fpGetSwapchainImagesKHR(device, swapChain, &imageCount, images.data());
        if (ret != VK_SUCCESS) {
            return VKBP_ERR(ret);
        }

        // make buffers for swap chains
        buffers.resize(imageCount);
        for (uint32_t i = 0; i < imageCount; i++)
        {
            VkImageViewCreateInfo colorAttachmentView = {};
            colorAttachmentView.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            colorAttachmentView.pNext = NULL;
            colorAttachmentView.format = colorFormat;
            colorAttachmentView.components = {
                    VK_COMPONENT_SWIZZLE_R,
                    VK_COMPONENT_SWIZZLE_G,
                    VK_COMPONENT_SWIZZLE_B,
                    VK_COMPONENT_SWIZZLE_A
            };
            colorAttachmentView.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            colorAttachmentView.subresourceRange.baseMipLevel = 0;
            colorAttachmentView.subresourceRange.levelCount = 1;
            colorAttachmentView.subresourceRange.baseArrayLayer = 0;
            colorAttachmentView.subresourceRange.layerCount = 1;
            colorAttachmentView.viewType = VK_IMAGE_VIEW_TYPE_2D;
            colorAttachmentView.flags = 0;

            buffers[i].image = images[i];

            // Transform images from initial (undefined) to present layout
            vkTools::setImageLayout(
                    cmdBuffer,
                    buffers[i].image,
                    VK_IMAGE_ASPECT_COLOR_BIT,
                    VK_IMAGE_LAYOUT_UNDEFINED,
                    VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

            VkImageSubresourceRange subresourceRange = {};
            subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            subresourceRange.baseMipLevel = 0;
            subresourceRange.levelCount = 1;
            subresourceRange.layerCount = 1;
            setImageLayout(cmdbuffer, image, aspectMask, oldImageLayout, newImageLayout, subresourceRange);

            //region setImageLayout

            // Create an image barrier object
            VkImageMemoryBarrier imageMemoryBarrier = vkTools::initializers::imageMemoryBarrier();
            imageMemoryBarrier.oldLayout = oldImageLayout;
            imageMemoryBarrier.newLayout = newImageLayout;
            imageMemoryBarrier.image = image;
            imageMemoryBarrier.subresourceRange = subresourceRange;

            // Source layouts (old)

            // Undefined layout
            // Only allowed as initial layout!
            // Make sure any writes to the image have been finished
            if (oldImageLayout == VK_IMAGE_LAYOUT_PREINITIALIZED)
            {
                imageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
            }

            // Old layout is color attachment
            // Make sure any writes to the color buffer have been finished
            if (oldImageLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
            {
                imageMemoryBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            }

            // Old layout is depth/stencil attachment
            // Make sure any writes to the depth/stencil buffer have been finished
            if (oldImageLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
            {
                imageMemoryBarrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            }

            // Old layout is transfer source
            // Make sure any reads from the image have been finished
            if (oldImageLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)
            {
                imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            }

            // Old layout is shader read (sampler, input attachment)
            // Make sure any shader reads from the image have been finished
            if (oldImageLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
            {
                imageMemoryBarrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
            }

            // Target layouts (new)

            // New layout is transfer destination (copy, blit)
            // Make sure any copyies to the image have been finished
            if (newImageLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
            {
                imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            }

            // New layout is transfer source (copy, blit)
            // Make sure any reads from and writes to the image have been finished
            if (newImageLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)
            {
                imageMemoryBarrier.srcAccessMask = imageMemoryBarrier.srcAccessMask | VK_ACCESS_TRANSFER_READ_BIT;
                imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            }

            // New layout is color attachment
            // Make sure any writes to the color buffer hav been finished
            if (newImageLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
            {
                imageMemoryBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
                imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            }

            // New layout is depth attachment
            // Make sure any writes to depth/stencil buffer have been finished
            if (newImageLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
            {
                imageMemoryBarrier.dstAccessMask = imageMemoryBarrier.dstAccessMask | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            }

            // New layout is shader read (sampler, input attachment)
            // Make sure any writes to the image have been finished
            if (newImageLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
            {
                imageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
                imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
            }

            // Put barrier on top
            VkPipelineStageFlags srcStageFlags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            VkPipelineStageFlags destStageFlags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;

            // Put barrier inside setup command buffer
            vkCmdPipelineBarrier(
                    cmdbuffer,
                    srcStageFlags,
                    destStageFlags,
                    0,
                    0, nullptr,
                    0, nullptr,
                    1, &imageMemoryBarrier);

            //endregion

            colorAttachmentView.image = buffers[i].image;

            ret = vkCreateImageView(device, &colorAttachmentView, nullptr, &buffers[i].view);
            if (ret != VK_SUCCESS) {
                return VKBP_ERR(ret);
            }
        }

        //endregion

        std::getchar();
        return VKBP_SUCCESS;
    }
}
