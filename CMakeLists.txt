
# Galen Cochrane
# Copyright (C) 2016
# This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)

# No lesser version of cMake may be used.
cmake_minimum_required(VERSION 3.3)

# Declare the project in question
project(VKBP)

# Tell cMake where to look for the modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

IF(WIN32 OR CYGWIN)
    message("Building for Windows")
    set(VULKAN_ROOT $ENV{VULKAN_SDK})
    set(LIBS ${LIBS} "${VULKAN_ROOT}/Bin32/vulkan-1.lib")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVK_USE_PLATFORM_WIN32_KHR")
    set(INCLUDE_DIR ${INCLUDE_DIR} "${VULKAN_ROOT}/Include")
ELSE(WIN32 OR CYGWIN)
    message("Building for Linux")
    set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} "/usr/lib64")
    find_library(VULKAN_LIBS NAMES libvulkan.so PATHS ${CMAKE_LIBRARY_PATH})
    find_package(XCB REQUIRED)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVK_USE_PLATFORM_XCB_KHR -std=c++11 -Wall -Wextra -g -O3")
    set(LIBS ${LIBS} ${VULKAN_LIBS})
ENDIF(WIN32 OR CYGWIN)

set(INCLUDE_DIR ${INCLUDE_DIR} ./)
include_directories(${INCLUDE_DIR})

# Find all the source files
set(SOURCE_FILES vulkanMain.cpp vulkanHelpers.cpp vulkanHelpers.h VulkanContext.cpp VulkanContext.h vkbpGlobal.h vkbpGlobal.cpp vkbp.h VkbpWindow.cpp VkbpWindow.h)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} ${LIBS})