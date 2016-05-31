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
#include "vulkan/vulkan.h"
//#include "vkbpGlobal.h"
#include "Vkbp.h"
#include "vkbpHelpers.h"

#ifdef WIN32
#include <cstdio>
#endif

namespace vkbp {

    Vkbp::Vkbp() : instance(0), hasInit(false), setupCmdBuffer(VK_NULL_HANDLE), swapChain(VK_NULL_HANDLE),
               width(1280), height(720), imageCount(0) {

    }

    Vkbp::~Vkbp() {
        if (hasInit) {
            vkDestroyInstance(instance, NULL);
        }
    }

    VkbpResult Vkbp::init(const char* appName, const char* engName) {

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

    VkbpResult Vkbp::initSimple() {

        //region lotsOfStuff

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

        //endregion

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

        return VKBP_SUCCESS;
    }






    //region Platform
    #if defined( _WIN32 )
    #define VK_USE_PLATFORM_WIN32_KHR 1
    #define PLATFORM_SURFACE_EXTENSION_NAME VK_KHR_WIN32_SURFACE_EXTENSION_NAME
    #include <Windows.h>
    #elif defined( __linux )
    #define VK_USE_PLATFORM_XCB_KHR 1
    #define PLATFORM_SURFACE_EXTENSION_NAME VK_KHR_XCB_SURFACE_EXTENSION_NAME
    #include <xcb/xcb.h>
    #endif
    //endregion

    //region VulkanDebugCallback
    VKAPI_ATTR VkBool32 VKAPI_CALL
    VulkanDebugCallback(
            VkDebugReportFlagsEXT		flags,
            VkDebugReportObjectTypeEXT	obj_type,
            uint64_t					src_obj,
            size_t						location,
            int32_t						msg_code,
            const char *				layer_prefix,
            const char *				msg,
            void *						user_data
    )
    {
        std::ostringstream stream;
        stream << "VKDBG: ";
        if( flags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT ) {
            stream << "INFO: ";
        }
        if( flags & VK_DEBUG_REPORT_WARNING_BIT_EXT ) {
            stream << "WARNING: ";
        }
        if( flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT ) {
            stream << "PERFORMANCE: ";
        }
        if( flags & VK_DEBUG_REPORT_ERROR_BIT_EXT ) {
            stream << "ERROR: ";
        }
        if( flags & VK_DEBUG_REPORT_DEBUG_BIT_EXT ) {
            stream << "DEBUG: ";
        }
        stream << "@[" << layer_prefix << "]: ";
        stream << msg << std::endl;
        std::cout << stream.str();

#ifdef _WIN32
        if( flags & VK_DEBUG_REPORT_ERROR_BIT_EXT ) {
		MessageBox( NULL, stream.str().c_str(), "Vulkan Error!", 0 );
	}
#endif

        return false;
    }
    //endregion

    VkbpResult Vkbp::initSimple2() {
        VkResult result;

        //region RendererMembers
        VkInstance								_instance						= VK_NULL_HANDLE;
        VkPhysicalDevice						_gpu							= VK_NULL_HANDLE;
        VkDevice								_device							= VK_NULL_HANDLE;
        VkQueue									_queue							= VK_NULL_HANDLE;
        VkPhysicalDeviceProperties				_gpu_properties					= {};
        uint32_t								_graphics_family_index			= 0;
        std::vector<const char*>				_instance_layers;
        std::vector<const char*>				_instance_extensions;
        std::vector<const char*>				_device_layers;
        std::vector<const char*>				_device_extensions;
        VkDebugReportCallbackEXT				_debug_report					= VK_NULL_HANDLE;

        VkApplicationInfo                       application_info {};
        VkInstanceCreateInfo                    instance_create_info {};
        VkDebugReportCallbackCreateInfoEXT		_debug_callback_create_info		= {};
        //endregion
        //region WindowMembers
        VkSurfaceKHR						_surface						= VK_NULL_HANDLE;
        VkSwapchainKHR						_swapchain						= VK_NULL_HANDLE;
        uint32_t							_surface_size_x					= 512;
        uint32_t							_surface_size_y					= 512;
        std::string							_window_name;
        uint32_t							_swapchain_image_count			= 2;
        VkSurfaceFormatKHR					_surface_format					= {};
        VkSurfaceCapabilitiesKHR			_surface_capabilities			= {};
        bool								_window_should_run				= true;
#if VK_USE_PLATFORM_WIN32_KHR
        HINSTANCE							_win32_instance					= NULL;
	HWND								_win32_window					= NULL;
	std::string							_win32_class_name;
	static uint64_t						_win32_class_id_counter;
#elif VK_USE_PLATFORM_XCB_KHR
        xcb_connection_t				*	_xcb_connection					= nullptr;
        xcb_screen_t					*	_xcb_screen						= nullptr;
        xcb_window_t						_xcb_window						= 0;
        xcb_intern_atom_reply_t			*	_xcb_atom_window_reply			= nullptr;
#endif
        //endregion
        //region FunctionPointerMembers
        PFN_vkCreateDebugReportCallbackEXT		fvkCreateDebugReportCallbackEXT		= nullptr;
        PFN_vkDestroyDebugReportCallbackEXT		fvkDestroyDebugReportCallbackEXT	= nullptr;
        //endregion
        //region SetupLayersAndExtensions
        _instance_extensions.push_back( VK_KHR_SURFACE_EXTENSION_NAME );
        _instance_extensions.push_back( PLATFORM_SURFACE_EXTENSION_NAME );
        _device_extensions.push_back( VK_KHR_SWAPCHAIN_EXTENSION_NAME );
        //endregion
        //region SetupDebug
        _debug_callback_create_info.sType			= VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
        _debug_callback_create_info.pfnCallback		= VulkanDebugCallback;
        _debug_callback_create_info.flags			=
		        VK_DEBUG_REPORT_INFORMATION_BIT_EXT         |
                VK_DEBUG_REPORT_WARNING_BIT_EXT             |
                VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT |
                VK_DEBUG_REPORT_ERROR_BIT_EXT               |
                VK_DEBUG_REPORT_DEBUG_BIT_EXT               ;
        _instance_layers.push_back( "VK_LAYER_LUNARG_standard_validation" );
        _instance_extensions.push_back( VK_EXT_DEBUG_REPORT_EXTENSION_NAME );
        _device_layers.push_back( "VK_LAYER_LUNARG_standard_validation" );
        //endregion
        //region InitInstance
        application_info.sType							= VK_STRUCTURE_TYPE_APPLICATION_INFO;
        application_info.apiVersion						= VK_MAKE_VERSION( 1, 0, 2 );			// 1.0.2 should work on all vulkan enabled drivers.
        application_info.applicationVersion				= VK_MAKE_VERSION( 0, 1, 0 );
        application_info.pApplicationName				= "Vulkan API Tutorial Series";

        instance_create_info.sType						= VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instance_create_info.pApplicationInfo			= &application_info;
        instance_create_info.enabledLayerCount			= (uint32_t)_instance_layers.size();
        instance_create_info.ppEnabledLayerNames		= _instance_layers.data();
        instance_create_info.enabledExtensionCount		= (uint32_t)_instance_extensions.size();
        instance_create_info.ppEnabledExtensionNames	= _instance_extensions.data();
        instance_create_info.pNext						= &_debug_callback_create_info;

        result = vkCreateInstance(&instance_create_info, nullptr, &_instance);
        VKBP_CHECK_ERR(result);
        //endregion
        //region InitDebug
        fvkCreateDebugReportCallbackEXT		= (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr( _instance, "vkCreateDebugReportCallbackEXT" );
        fvkDestroyDebugReportCallbackEXT	= (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr( _instance, "vkDestroyDebugReportCallbackEXT" );
        if(!fvkCreateDebugReportCallbackEXT || !fvkDestroyDebugReportCallbackEXT) {
            return VKBP_MSG("Unable to retrieve function pointers!");
        }
        fvkCreateDebugReportCallbackEXT( _instance, &_debug_callback_create_info, nullptr, &_debug_report );
        //endregion
        //region FindGPU
        uint32_t physDeviceCount = 0;
        result = vkEnumeratePhysicalDevices(_instance, &physDeviceCount, nullptr);
        VKBP_CHECK_ERR(result);
        if (!physDeviceCount) {
            return VKBP_MSG("No GPU found!");
        }
        std::vector<VkPhysicalDevice> physDeviceList(physDeviceCount);
        result = vkEnumeratePhysicalDevices(_instance, &physDeviceCount, physDeviceList.data());
        VKBP_CHECK_ERR(result);
        int whichPhysicalDevice = 0;
        VkPhysicalDeviceType currentPhysDeviceType = VK_PHYSICAL_DEVICE_TYPE_OTHER;
        std::cout << "Physical devices recognized: (" << physDeviceCount << " total): \n    ----\n";
        for (uint32_t i = 0; i < physDeviceCount; ++i) {
            vkGetPhysicalDeviceProperties(physDeviceList[i], &_gpu_properties);
            std::cout << "    NAME: " << _gpu_properties.deviceName << std::endl;
            std::cout << "    TYPE: " << resolvePhysicalDeviceTypeToString(_gpu_properties.deviceType);
            std::cout << "\n    ----\n";
            if (    (currentPhysDeviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
                        currentPhysDeviceType != VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU &&
                        _gpu_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU) ||
                    (currentPhysDeviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
                        _gpu_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU))
            {
                whichPhysicalDevice = i;
                currentPhysDeviceType = _gpu_properties.deviceType;
            }
        }
        _gpu = physDeviceList[whichPhysicalDevice];
        vkGetPhysicalDeviceProperties( _gpu, &_gpu_properties );
        std::cout << "Choosing to use: " << _gpu_properties.deviceName << std::endl;
        //endregion
        //region FindGraphicsQueue
        uint32_t familyCount;
        vkGetPhysicalDeviceQueueFamilyProperties(_gpu, &familyCount, NULL);
        if (familyCount < 1) {
            return VKBP_MSG("Queue family properties not found!");
        }
        std::vector<VkQueueFamilyProperties> queueFamilyList(familyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(_gpu, &familyCount, queueFamilyList.data());
        for (_graphics_family_index = 0; _graphics_family_index < familyCount; ++_graphics_family_index) {
            if (queueFamilyList[_graphics_family_index].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                break;
            }
        }
        if (_graphics_family_index >= familyCount) {
            return VKBP_MSG("Graphics queue not found!");
        }
        //endregion
        //region CreateDevice
        float queue_priorities[] { 0.f };
        VkDeviceQueueCreateInfo device_queue_create_info {};
        device_queue_create_info.sType				= VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        device_queue_create_info.queueFamilyIndex	= _graphics_family_index;
        device_queue_create_info.queueCount			= 1;
        device_queue_create_info.pQueuePriorities	= queue_priorities;

        VkDeviceCreateInfo device_create_info {};
        device_create_info.sType					= VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        device_create_info.queueCreateInfoCount		= 1;
        device_create_info.pQueueCreateInfos		= &device_queue_create_info;
        device_create_info.enabledLayerCount		= _device_layers.size();
        device_create_info.ppEnabledLayerNames		= _device_layers.data();
        device_create_info.enabledExtensionCount	= _device_extensions.size();
        device_create_info.ppEnabledExtensionNames	= _device_extensions.data();

        result = vkCreateDevice(_gpu, &device_create_info, nullptr, &_device);
        VKBP_CHECK_ERR(result);
        vkGetDeviceQueue(_device, _graphics_family_index, 0, &_queue);
        //endregion

        return VKBP_SUCCESS;
    }

}
