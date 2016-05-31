/**
 * part of VKBP (Vulkan Boilerplate)
 *
 * Galen Cochrane
 * Copyright (C) 2016
 * This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
 */

#include <unistd.h>
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
        std::vector<VkImage>				_swapchain_images;
        std::vector<VkImageView>			_swapchain_image_views;
        VkSurfaceFormatKHR					_surface_format					= {};
        VkSurfaceCapabilitiesKHR			_surface_capabilities			= {};
        GLFWwindow						*	_glfw_window					= nullptr;
        //endregion
        //region FunctionPointerMembers
        PFN_vkCreateDebugReportCallbackEXT		fvkCreateDebugReportCallbackEXT		= nullptr;
        PFN_vkDestroyDebugReportCallbackEXT		fvkDestroyDebugReportCallbackEXT	= nullptr;
        //endregion

        //region InitPlatform
        glfwInit();
        if( glfwVulkanSupported() == GLFW_FALSE ) {
            return VKBP_MSG("GLFW Vulkan support not found.");
        }
        //endregion

        //region SetupLayersAndExtensions
        _instance_extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
        uint32_t instance_extension_count = 0;
        const char ** instance_extensions_buffer = glfwGetRequiredInstanceExtensions(&instance_extension_count);
        for (uint32_t i=0; i < instance_extension_count; i++){
            _instance_extensions.push_back(instance_extensions_buffer[i]);
        }
        _device_extensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
        //endregion
        //region SetupDebug
        _debug_callback_create_info.sType			= VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
        _debug_callback_create_info.pfnCallback		= VkbpDebugCallback;
        _debug_callback_create_info.flags			= VK_DEBUG_REPORT_ERROR_BIT_EXT |
                                                      VK_DEBUG_REPORT_WARNING_BIT_EXT;
        if (VKBP_VERBOSE_DEBUG) {
            _debug_callback_create_info.flags      |= VK_DEBUG_REPORT_INFORMATION_BIT_EXT         |
                                                      VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT |
                                                      VK_DEBUG_REPORT_DEBUG_BIT_EXT               ;
        }
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

        //region InitWindow
        glfwWindowHint( GLFW_CLIENT_API, GLFW_NO_API );
        _glfw_window = glfwCreateWindow( _surface_size_x, _surface_size_y, _window_name.c_str(), nullptr, nullptr );
        glfwGetFramebufferSize ( _glfw_window, (int*)&_surface_size_x, (int*)&_surface_size_y );
        //endregion
        //region InitSurface
        result = glfwCreateWindowSurface( _instance, _glfw_window, nullptr, &_surface );
        VKBP_CHECK_ERR(result);

        VkBool32 WSI_supported = (VkBool32)false;
        vkGetPhysicalDeviceSurfaceSupportKHR( _gpu, _graphics_family_index, _surface, &WSI_supported );
        if( !WSI_supported ) {
            return VKBP_MSG("WSI not supported");
        }

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR( _gpu, _surface, &_surface_capabilities );
        if( _surface_capabilities.currentExtent.width < UINT32_MAX ) {
            _surface_size_x			= _surface_capabilities.currentExtent.width;
            _surface_size_y			= _surface_capabilities.currentExtent.height;
        }

        {
            uint32_t format_count = 0;
            vkGetPhysicalDeviceSurfaceFormatsKHR( _gpu, _surface, &format_count, nullptr );
            if( format_count == 0 ) {
                return VKBP_MSG("Surface formats missing.");
            }
            std::vector<VkSurfaceFormatKHR> formats( format_count );
            vkGetPhysicalDeviceSurfaceFormatsKHR( _gpu, _surface, &format_count, formats.data() );
            if( formats[ 0 ].format == VK_FORMAT_UNDEFINED ) {
                _surface_format.format		= VK_FORMAT_B8G8R8A8_UNORM;
                _surface_format.colorSpace	= VK_COLORSPACE_SRGB_NONLINEAR_KHR;
            } else {
                _surface_format				= formats[ 0 ];
            }
        }
        //endregion
        //region InitSwapchain
        if( _swapchain_image_count < _surface_capabilities.minImageCount + 1 ) _swapchain_image_count = _surface_capabilities.minImageCount + 1;
        if( _surface_capabilities.maxImageCount > 0 ) {
            if( _swapchain_image_count > _surface_capabilities.maxImageCount ) _swapchain_image_count = _surface_capabilities.maxImageCount;
        }

        VkPresentModeKHR present_mode = VK_PRESENT_MODE_FIFO_KHR;
        {
            uint32_t present_mode_count = 0;
            result = vkGetPhysicalDeviceSurfacePresentModesKHR(_gpu, _surface, &present_mode_count, nullptr);
            VKBP_CHECK_ERR(result);
            std::vector<VkPresentModeKHR> present_mode_list( present_mode_count );
            result = vkGetPhysicalDeviceSurfacePresentModesKHR(_gpu, _surface, &present_mode_count, present_mode_list.data());
            VKBP_CHECK_ERR(result);
            for( auto m : present_mode_list ) {
                if( m == VK_PRESENT_MODE_MAILBOX_KHR ) present_mode = m;
            }
        }

        VkSwapchainCreateInfoKHR swapchain_create_info {};
        swapchain_create_info.sType						= VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapchain_create_info.surface					= _surface;
        swapchain_create_info.minImageCount				= _swapchain_image_count;
        swapchain_create_info.imageFormat				= _surface_format.format;
        swapchain_create_info.imageColorSpace			= _surface_format.colorSpace;
        swapchain_create_info.imageExtent.width			= _surface_size_x;
        swapchain_create_info.imageExtent.height		= _surface_size_y;
        swapchain_create_info.imageArrayLayers			= 1;
        swapchain_create_info.imageUsage				= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        swapchain_create_info.imageSharingMode			= VK_SHARING_MODE_EXCLUSIVE;
        swapchain_create_info.queueFamilyIndexCount		= 0;
        swapchain_create_info.pQueueFamilyIndices		= nullptr;
        swapchain_create_info.preTransform				= VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
        swapchain_create_info.compositeAlpha			= VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        swapchain_create_info.presentMode				= present_mode;
        swapchain_create_info.clipped					= VK_TRUE;
        swapchain_create_info.oldSwapchain				= VK_NULL_HANDLE;

        result = vkCreateSwapchainKHR(_device, &swapchain_create_info, nullptr, &_swapchain);
        VKBP_CHECK_ERR(result);

        result = vkGetSwapchainImagesKHR(_device, _swapchain, &_swapchain_image_count, nullptr);
        VKBP_CHECK_ERR(result);
        //endregion
        //region InitSwapchainImages
        _swapchain_images.resize( _swapchain_image_count );
        _swapchain_image_views.resize( _swapchain_image_count );

        result = vkGetSwapchainImagesKHR(_device, _swapchain, &_swapchain_image_count, _swapchain_images.data());
        VKBP_CHECK_ERR(result);

        for( uint32_t i=0; i < _swapchain_image_count; ++i ) {
            VkImageViewCreateInfo image_view_create_info {};
            image_view_create_info.sType				= VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            image_view_create_info.image				= _swapchain_images[ i ];
            image_view_create_info.viewType				= VK_IMAGE_VIEW_TYPE_2D;
            image_view_create_info.format				= _surface_format.format;
            image_view_create_info.components.r			= VK_COMPONENT_SWIZZLE_IDENTITY;
            image_view_create_info.components.g			= VK_COMPONENT_SWIZZLE_IDENTITY;
            image_view_create_info.components.b			= VK_COMPONENT_SWIZZLE_IDENTITY;
            image_view_create_info.components.a			= VK_COMPONENT_SWIZZLE_IDENTITY;
            image_view_create_info.subresourceRange.aspectMask			= VK_IMAGE_ASPECT_COLOR_BIT;
            image_view_create_info.subresourceRange.baseMipLevel		= 0;
            image_view_create_info.subresourceRange.levelCount			= 1;
            image_view_create_info.subresourceRange.baseArrayLayer		= 0;
            image_view_create_info.subresourceRange.layerCount			= 1;

            result = vkCreateImageView(_device, &image_view_create_info, nullptr, &_swapchain_image_views[i]);
            VKBP_CHECK_ERR_MSG(result, std::string("(iteration " + std::to_string(i)).c_str());
        }
        //endregion

        //region mainloop
        bool shouldRun = true;
        while (shouldRun) {
            if (_glfw_window) {
                glfwPollEvents();
                if (glfwWindowShouldClose(_glfw_window)) {
                    shouldRun = false;
                    std::cout << "EXIT\n";
                }
            }
        }
        //endregion

        //region DestroySwapchainImages
        for(auto view : _swapchain_image_views) {
            vkDestroyImageView(_device, view, nullptr);
        }
        //endregion
        //region DestroySwapchain
        vkDestroySwapchainKHR(_device, _swapchain, nullptr);
        //endregion
        //region DestroySurface
        vkDestroySurfaceKHR(_instance, _surface, nullptr);
        //endregion
        //region DestroyWindow
        glfwDestroyWindow(_glfw_window);
        //endregion

        //region DestroyDevice
        vkDestroyDevice( _device, nullptr );
        _device = nullptr;
        //endregion
        //region DestroyDebug
        fvkDestroyDebugReportCallbackEXT( _instance, _debug_report, nullptr );
        _debug_report = VK_NULL_HANDLE;
        //endregion
        //region DestroyInstance
        vkDestroyInstance( _instance, nullptr );
        _instance = nullptr;
        //endregion

        //region DestroyPlatform
        glfwTerminate();
        //endregion

        return VKBP_SUCCESS;
    }
}
