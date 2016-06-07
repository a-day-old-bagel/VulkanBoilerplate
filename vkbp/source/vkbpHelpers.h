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

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#define VKBP_ERR_MSG(res, msg) VkbpResult(__FILENAME__, __func__, __LINE__, res, msg)
#define VKBP_ERR(res) VkbpResult(__FILENAME__, __func__, __LINE__, res)
#define VKBP_MSG(msg) VkbpResult(__FILENAME__, __func__, __LINE__, VK_RESULT_MAX_ENUM, msg)
#define VKBP_SUCCESS VkbpResult();

#define VKBP_CHECK_ERR_MSG(res, msg) if (res != VK_SUCCESS) return VKBP_ERR_MSG(res, msg)
#define VKBP_CHECK_ERR(res) if (res != VK_SUCCESS) { return VKBP_ERR(res); }
#define VKBP_CHECK_MSG(res, msg) if (res != VK_SUCCESS) { return VKBP_MSG(msg); }

namespace vkbp {

    std::string resolveErrorToString(VkResult err);
    std::string resolvePhysicalDeviceTypeToString(VkPhysicalDeviceType type);

    VKAPI_ATTR VkBool32 VKAPI_CALL
    VkbpDebugCallback(
            VkDebugReportFlagsEXT flags,
            VkDebugReportObjectTypeEXT objType,
            uint64_t sourceObject,
            size_t location,
            int32_t messageCode,
            const char* layerPrefix,
            const char* message,
            void* userData);

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
