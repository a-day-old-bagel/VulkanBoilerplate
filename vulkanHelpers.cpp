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
        instanceInfo.enabledLayerCount = settings::validation_layer_count;
        instanceInfo.ppEnabledLayerNames = (const char**)settings::validation_layer_names;
    }

    void AppendBasicVkExtensions(std::vector<const char*> &extVec) {
        extVec.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
        extVec.push_back(settings::windowing_system_extension_name);
        if (settings::validation_layer_count) {
            extVec.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
        }
    }

    #define CASE_ENUM_TOSTR(r) case VK_ ##r: return #r
    std::string resolveEnumToString(VkResult err) {
        switch (err) {
            CASE_ENUM_TOSTR(NOT_READY);
            CASE_ENUM_TOSTR(TIMEOUT);
            CASE_ENUM_TOSTR(EVENT_SET);
            CASE_ENUM_TOSTR(EVENT_RESET);
            CASE_ENUM_TOSTR(INCOMPLETE);
            CASE_ENUM_TOSTR(ERROR_OUT_OF_HOST_MEMORY);
            CASE_ENUM_TOSTR(ERROR_OUT_OF_DEVICE_MEMORY);
            CASE_ENUM_TOSTR(ERROR_INITIALIZATION_FAILED);
            CASE_ENUM_TOSTR(ERROR_DEVICE_LOST);
            CASE_ENUM_TOSTR(ERROR_MEMORY_MAP_FAILED);
            CASE_ENUM_TOSTR(ERROR_LAYER_NOT_PRESENT);
            CASE_ENUM_TOSTR(ERROR_EXTENSION_NOT_PRESENT);
            CASE_ENUM_TOSTR(ERROR_INCOMPATIBLE_DRIVER);
            CASE_ENUM_TOSTR(ERROR_FEATURE_NOT_PRESENT);
            CASE_ENUM_TOSTR(ERROR_TOO_MANY_OBJECTS);
            CASE_ENUM_TOSTR(ERROR_FORMAT_NOT_SUPPORTED);
            CASE_ENUM_TOSTR(ERROR_SURFACE_LOST_KHR);
            CASE_ENUM_TOSTR(ERROR_NATIVE_WINDOW_IN_USE_KHR);
            CASE_ENUM_TOSTR(SUBOPTIMAL_KHR);
            CASE_ENUM_TOSTR(ERROR_OUT_OF_DATE_KHR);
            CASE_ENUM_TOSTR(ERROR_INCOMPATIBLE_DISPLAY_KHR);
            CASE_ENUM_TOSTR(ERROR_VALIDATION_FAILED_EXT);
            CASE_ENUM_TOSTR(ERROR_INVALID_SHADER_NV);
            default:
                return "Unknown Error.";
        }
    }
    #undef CASE_ENUM_TOSTR
}
