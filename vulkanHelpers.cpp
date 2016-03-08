/**
 * part of VKBP (Vulkan Boilerplate)
 *
 * Galen Cochrane
 * Copyright (C) 2016
 * This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
 */

#include "vulkan/vulkan.h"
#include "vkbpDefines.h"
#include "vulkanHelpers.h"

namespace vkbp {

    void FillVkApplicationInfo(const char* appName, const char* engName, VkApplicationInfo &appInfo) {
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = appName;
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = engName;
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_MAKE_VERSION(1, 0, 3);
    }

    void FillVkInstanceInfo(const VkApplicationInfo &appInfo, const std::vector<const char*> &extVec,
                            VkInstanceCreateInfo &instanceInfo) {
        instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instanceInfo.pNext = NULL;
        instanceInfo.flags = 0;
        instanceInfo.pApplicationInfo = &appInfo;
        instanceInfo.enabledExtensionCount = (uint32_t) extVec.size();
        if (instanceInfo.enabledExtensionCount) {
            instanceInfo.ppEnabledExtensionNames = extVec.data();
        } else {
            instanceInfo.ppEnabledExtensionNames = NULL;
        }
        instanceInfo.enabledLayerCount = settings::validation_layer_count;
        if (settings::validation_layer_count) {
            instanceInfo.ppEnabledLayerNames = settings::validation_layer_names;
        } else {
            instanceInfo.ppEnabledLayerNames = NULL;
        }
    }

    void AppendBasicVkExtensions(std::vector<const char*> &extVec) {
        extVec.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
        extVec.push_back(settings::windowing_system_extension_name);
        if (settings::validation_layer_count) {
            extVec.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
        }
    }

    #define STR(r) case VK_ ##r: return #r
    std::string errorString(VkResult errorCode) {
        switch (errorCode) {
            STR(NOT_READY);
            STR(TIMEOUT);
            STR(EVENT_SET);
            STR(EVENT_RESET);
            STR(INCOMPLETE);
            STR(ERROR_OUT_OF_HOST_MEMORY);
            STR(ERROR_OUT_OF_DEVICE_MEMORY);
            STR(ERROR_INITIALIZATION_FAILED);
            STR(ERROR_DEVICE_LOST);
            STR(ERROR_MEMORY_MAP_FAILED);
            STR(ERROR_LAYER_NOT_PRESENT);
            STR(ERROR_EXTENSION_NOT_PRESENT);
            STR(ERROR_INCOMPATIBLE_DRIVER);
            default:
                return "UNKNOWN_ERROR";
        }
    }
    #undef STR
}
