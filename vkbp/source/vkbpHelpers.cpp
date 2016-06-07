/**
 * part of VKBP (Vulkan Boilerplate)
 *
 * Galen Cochrane
 * Copyright (C) 2016
 * This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
 */

#include <sstream>
#include <iostream>
#include "vulkan/vulkan.h"
#include "vkbpGlobal.h"
#include "vkbpHelpers.h"

namespace vkbp {

    VKAPI_ATTR VkBool32 VKAPI_CALL
    VkbpDebugCallback(
            VkDebugReportFlagsEXT flags,
            VkDebugReportObjectTypeEXT objType,
            uint64_t sourceObject,
            size_t location,
            int32_t messageCode,
            const char* layerPrefix,
            const char* message,
            void* userData
    )
    {
        std::ostringstream out;
        if( flags & VK_DEBUG_REPORT_ERROR_BIT_EXT ) {
            out << "ERROR: ";
        }
        if( flags & VK_DEBUG_REPORT_WARNING_BIT_EXT ) {
            out << "WARNING: ";
        }
        if (flags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT) {
            out << "INFO: ";
        }
        if (flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT) {
            out << "PERFORMANCE: ";
        }
        if (flags & VK_DEBUG_REPORT_DEBUG_BIT_EXT) {
            out << "DEBUG: ";
        }
        out << "(" << layerPrefix << ", Code " << messageCode << ") " << message << std::endl;
        std::cout << out.str();
        fflush(stdout);

        return (VkBool32)false;
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
            case VK_RESULT_MAX_ENUM:
                return "Error";
                break;
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

    VkbpResult::VkbpResult(const char* message /*= ""*/)
            : lineNumber(-1), errCode(VK_SUCCESS), fileName(""), funcName(""), message(message) { }

    VkbpResult::VkbpResult(const char* fileName, const char* funcName, int lineNumber, VkResult errCode,
                           const char* message /*= ""*/)
            : lineNumber(lineNumber), errCode(errCode), fileName(fileName), funcName(funcName), message(message) { }

    std::string VkbpResult::toString() {
        return resolveErrorToString(errCode) + " found in file \'" + fileName + "\'" +
                (funcName.length() ? " in function " : "" ) + funcName +
                " at line " + std::to_string(lineNumber) +
                (message.length() ? " with message: " : "") + message;
    }

}

