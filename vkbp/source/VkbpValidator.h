//
// Created by volundr on 6/8/16.
//

#ifndef VKBP_VKBPDEBUG_H
#define VKBP_VKBPDEBUG_H

#include "vulkan/vulkan.h"
#include "vkbpGlobal.h"
#include "vkbpHelpers.h"

namespace vkbp {
    /**
     * The VKBP Validator handles everything relating to vulkan validation layers.
     * Via the use of the NDEBUG and VDEBUG flags it will compile with different functionality as you can see by
     * examining VkbpValidator.cpp.  For a release build, for example, the NDEBUG flag should be defined and as
     * a result VkbpValidator will have almost no functionality.
     */
    class VkbpValidator {

        VkDebugReportCallbackEXT debugReport;
        PFN_vkCreateDebugReportCallbackEXT createDebugReportCallback;
        PFN_vkDestroyDebugReportCallbackEXT destroyDebugReportCallback;
        VkDebugReportCallbackCreateInfoEXT debugReportCallbackCreateInfo;

    public:

        VkbpValidator();
        VkbpResult addValidationInfoBeforeInstanceCreation(std::vector<const char*>& instanceExtensions,
                                                           VkInstanceCreateInfo& instanceInfo);
        VkbpResult createDebugReportCallbackAfterInstanceCreation(VkInstance& instance);
        VkbpResult destroyDebugReportCallbackBeforeInstanceDestruction(VkInstance& instance);
    };
}

#endif //VKBP_VKBPDEBUG_H
