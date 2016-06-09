//
// Created by volundr on 6/8/16.
//

#include <sstream>
#include <cstring>
#include "VkbpValidator.h"

// The NDEBUG switch should be defined for any release build.  It removes validation layers and other debug code.
#ifndef NDEBUG  // The following is code with debugging enabled

namespace vkbp {

    const std::vector<const char*> validationLayerNames = { "VK_LAYER_LUNARG_standard_validation" };

    /**/ // The VDEBUG switch should be defined if you want detailed validation output like performance validation.
    /**/ // If NDEBUG is also defined this will have no effect.
    /**/ #ifdef VDEBUG
    /**/ const int debugReportCallbackFlags = VK_DEBUG_REPORT_ERROR_BIT_EXT                 |
    /**/                                      VK_DEBUG_REPORT_WARNING_BIT_EXT               |
    /**/                                      VK_DEBUG_REPORT_INFORMATION_BIT_EXT           |
    /**/                                      VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT   |
    /**/                                      VK_DEBUG_REPORT_DEBUG_BIT_EXT                 ;
    /**/ #else //VDEBUG
    /**/ const int debugReportCallbackFlags = VK_DEBUG_REPORT_ERROR_BIT_EXT |
    /**/                                      VK_DEBUG_REPORT_WARNING_BIT_EXT;
    /**/ #endif //VDEBUG

    VKAPI_ATTR VkBool32 VKAPI_CALL
    debugReportCallback(
            VkDebugReportFlagsEXT flags,
            VkDebugReportObjectTypeEXT objType,
            uint64_t sourceObject,
            size_t location,
            int32_t messageCode,
            const char* layerPrefix,
            const char* message,
            void* userData) {
        std::ostringstream out;
        if (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT) {
            out << "[ ERROR ]   ";
        }
        if (flags & VK_DEBUG_REPORT_WARNING_BIT_EXT) {
            out << "[ WARNING ] ";
        }
        if (flags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT) {
            out << "[ INFO ]    ";
        }
        if (flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT) {
            out << "[ PERFORM ] ";
        }
        if (flags & VK_DEBUG_REPORT_DEBUG_BIT_EXT) {
            out << "[ DEBUG ]   ";
        }
        out << "(" << layerPrefix << ", " << messageCode << "): " << message << std::endl;
        printf("%s", out.str().c_str());
        fflush(stdout);

        return (VkBool32) false;
    }

    VkbpValidator::VkbpValidator()
            : debugReport(VK_NULL_HANDLE), createDebugReportCallback(nullptr), destroyDebugReportCallback(nullptr)
    {
        printf("Running with validation layers enabled.\n");
        debugReportCallbackCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
        debugReportCallbackCreateInfo.pfnCallback = debugReportCallback;
        debugReportCallbackCreateInfo.flags = (VkDebugReportFlagsEXT)debugReportCallbackFlags;
    }

    VkbpResult VkbpValidator::addValidationInfoBeforeInstanceCreation(std::vector<const char*> &instanceExtensions,
                                                                      VkInstanceCreateInfo &instanceInfo) {
        if (instanceExtensions.data() != instanceInfo.ppEnabledExtensionNames) {
            return VKBP_MSG("The instanceExtensions vector passed in is not being used as the ppEnabledExtensionsNames "
                                    "field of the passed instanceInfo (VkInstanceCreateInfo) struct.");
        }
        instanceExtensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
        instanceInfo.enabledExtensionCount = (uint32_t)instanceExtensions.size();
        instanceInfo.ppEnabledExtensionNames = instanceExtensions.data();
        instanceInfo.enabledLayerCount = (uint32_t) validationLayerNames.size();
        instanceInfo.ppEnabledLayerNames = validationLayerNames.data();
        // This enables validation before vkCreateInstance is called (they tell me it's a trick)
        instanceInfo.pNext = &debugReportCallbackCreateInfo;
        return VKBP_SUCCESS;
    }

    VkbpResult VkbpValidator::createDebugReportCallbackAfterInstanceCreation(VkInstance &instance) {
        createDebugReportCallback = (PFN_vkCreateDebugReportCallbackEXT) vkGetInstanceProcAddr(
                instance, "vkCreateDebugReportCallbackEXT");
        destroyDebugReportCallback = (PFN_vkDestroyDebugReportCallbackEXT) vkGetInstanceProcAddr(
                instance, "vkDestroyDebugReportCallbackEXT");
        if (!createDebugReportCallback || !destroyDebugReportCallback) {
            return VKBP_MSG("Unable to retrieve debug report function pointers!");
        }
        createDebugReportCallback(instance, &debugReportCallbackCreateInfo, nullptr, &debugReport);
        return VKBP_SUCCESS;
    }

    VkbpResult VkbpValidator::destroyDebugReportCallbackBeforeInstanceDestruction(VkInstance &instance) {
        destroyDebugReportCallback(instance, debugReport, nullptr);
        debugReport = VK_NULL_HANDLE;
        return VKBP_SUCCESS;
    }

}

#else //NDEBUG The following is code with debugging disabled

namespace vkbp {

    VkbpValidator::VkbpValidator() {
        printf("Running without validation layers.\n");
    }

    VkbpResult VkbpValidator::addValidationInfoBeforeInstanceCreation(std::vector<const char*> &instanceExtensions,
                                                                      VkInstanceCreateInfo &instanceInfo) {
        if (instanceExtensions.data() != instanceInfo.ppEnabledExtensionNames) {
            return VKBP_MSG("The instanceExtensions vector passed in is not being used as the ppEnabledExtensionsNames "
                                    "field of the passed instanceInfo (VkInstanceCreateInfo) struct.");
        }
        instanceInfo.enabledExtensionCount = (uint32_t)instanceExtensions.size();
        instanceInfo.ppEnabledExtensionNames = instanceExtensions.data();
        instanceInfo.enabledLayerCount = 0;
        instanceInfo.ppEnabledLayerNames = NULL;
        return VKBP_SUCCESS;
    }

    VkbpResult VkbpValidator::createDebugReportCallbackAfterInstanceCreation(VkInstance &instance) {
        return VKBP_SUCCESS;
    }

    VkbpResult VkbpValidator::destroyDebugReportCallbackBeforeInstanceDestruction(VkInstance &instance) {
        return VKBP_SUCCESS;
    }

}

#endif //NDEBUG