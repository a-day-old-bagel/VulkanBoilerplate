/**
 * part of VKBP (Vulkan Boilerplate)
 *
 * Galen Cochrane
 * Copyright (C) 2016
 * This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
 */

#include "vulkan/vulkan.h"
#include "vkbpGlobal.h"
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
        instanceInfo.enabledLayerCount = Settings::validation_layer_count;
        instanceInfo.ppEnabledLayerNames = (const char**)Settings::validation_layer_names;
    }

    void AppendBasicVkExtensions(std::vector<const char*> &extVec) {
        extVec.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
        extVec.push_back(Settings::windowing_system_extension_name);
        if (Settings::validation_layer_count) {
            extVec.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
        }
    }

    #define EMIT_CASE_ERROR_ENUM(e) case VK_ ##e: return #e
    std::string resolveErrorToString(VkResult err) {
        switch (err) {
            EMIT_CASE_ERROR_ENUM(NOT_READY);
            EMIT_CASE_ERROR_ENUM(TIMEOUT);
            EMIT_CASE_ERROR_ENUM(EVENT_SET);
            EMIT_CASE_ERROR_ENUM(EVENT_RESET);
            EMIT_CASE_ERROR_ENUM(INCOMPLETE);
            EMIT_CASE_ERROR_ENUM(ERROR_OUT_OF_HOST_MEMORY);
            EMIT_CASE_ERROR_ENUM(ERROR_OUT_OF_DEVICE_MEMORY);
            EMIT_CASE_ERROR_ENUM(ERROR_INITIALIZATION_FAILED);
            EMIT_CASE_ERROR_ENUM(ERROR_DEVICE_LOST);
            EMIT_CASE_ERROR_ENUM(ERROR_MEMORY_MAP_FAILED);
            EMIT_CASE_ERROR_ENUM(ERROR_LAYER_NOT_PRESENT);
            EMIT_CASE_ERROR_ENUM(ERROR_EXTENSION_NOT_PRESENT);
            EMIT_CASE_ERROR_ENUM(ERROR_INCOMPATIBLE_DRIVER);
            EMIT_CASE_ERROR_ENUM(ERROR_FEATURE_NOT_PRESENT);
            EMIT_CASE_ERROR_ENUM(ERROR_TOO_MANY_OBJECTS);
            EMIT_CASE_ERROR_ENUM(ERROR_FORMAT_NOT_SUPPORTED);
            EMIT_CASE_ERROR_ENUM(ERROR_SURFACE_LOST_KHR);
            EMIT_CASE_ERROR_ENUM(ERROR_NATIVE_WINDOW_IN_USE_KHR);
            EMIT_CASE_ERROR_ENUM(SUBOPTIMAL_KHR);
            EMIT_CASE_ERROR_ENUM(ERROR_OUT_OF_DATE_KHR);
            EMIT_CASE_ERROR_ENUM(ERROR_INCOMPATIBLE_DISPLAY_KHR);
            EMIT_CASE_ERROR_ENUM(ERROR_VALIDATION_FAILED_EXT);
            EMIT_CASE_ERROR_ENUM(ERROR_INVALID_SHADER_NV);
            default:
                return "Unknown Error";
        }
    }
    #undef EMIT_CASE_ERROR_ENUM

    #define EMIT_CASE_PHYSICAL_DEVICE_TYPE_ENUM(r) case VK_PHYSICAL_DEVICE_TYPE_ ##r: return #r
    std::string resolvePhysicalDeviceTypeToString(VkPhysicalDeviceType type) {
        switch(type) {
            EMIT_CASE_PHYSICAL_DEVICE_TYPE_ENUM(OTHER);
            EMIT_CASE_PHYSICAL_DEVICE_TYPE_ENUM(INTEGRATED_GPU);
            EMIT_CASE_PHYSICAL_DEVICE_TYPE_ENUM(DISCRETE_GPU);
            EMIT_CASE_PHYSICAL_DEVICE_TYPE_ENUM(VIRTUAL_GPU);
            EMIT_CASE_PHYSICAL_DEVICE_TYPE_ENUM(CPU);
            default:
                return "Unknown Type";
        }
    }
    #undef EMIT_CASE_PHYSICAL_DEVICE_TYPE_ENUM

}
