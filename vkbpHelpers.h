/**
 * part of VKBP (Vulkan Boilerplate)
 *
 * Galen Cochrane
 * Copyright (C) 2016
 * This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
 */

#include <vector>
#include <string>
#include "vulkan/vulkan.h"
#include "vkbpGlobal.h"

#ifndef VKBP_VULKANSTRUCTHELPERS_H
#define VKBP_VULKANSTRUCTHELPERS_H

//#define VKBP_FAIL VK_RESULT_MAX_ENUM
#define VKBP_ERR_MSG(res, msg) VkbpResult(__FILE__, __PRETTY_FUNCTION__, __LINE__, res, msg)
#define VKBP_ERR(res) VkbpResult(__FILE__, __PRETTY_FUNCTION__, __LINE__, res)
#define VKBP_MSG(msg) VkbpResult(__FILE__, __PRETTY_FUNCTION__, __LINE__, VK_RESULT_MAX_ENUM, msg)
#define VKBP_SUCCESS VkbpResult();

namespace vkbp {
    void AppendBasicVkExtensions(std::vector<const char*> &extVec);
    void FillVkApplicationInfo(const char* appName, const char* engName, VkApplicationInfo &appInfo);
    void FillVkInstanceInfo(const VkApplicationInfo &appInfo, const std::vector<const char*> &extVec,
                            VkInstanceCreateInfo &instanceInfo);
    std::string resolveErrorToString(VkResult err);
    std::string resolvePhysicalDeviceTypeToString(VkPhysicalDeviceType type);

    struct VkbpResult {
        int lineNumber;
        VkResult errCode;
        std::string fileName, funcName, message;
        VkbpResult(const char* message = "");
        VkbpResult(const char* fileName, const char* funcName, int lineNumber, VkResult errCode,
                   const char* message = "");
        std::string toString();
        inline bool isError() { return errCode != VK_SUCCESS; }
    };
}

#endif //VKBP_VULKANSTRUCTHELPERS_H
