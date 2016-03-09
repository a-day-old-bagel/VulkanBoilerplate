
# Galen Cochrane
# Copyright (C) 2016
# This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)

# No lesser version of cMake may be used.
cmake_minimum_required(VERSION 3.3)

# Declare the project in question
project(VKBP)

# Tell cMake where to look for the modules that search for SDL2 (the others come with cMake)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

# Tell cMake where to look for third party libs (add to this for your own builds)
IF(WIN32)
    message("Building for Windows")
    set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} "C:/libs/VulkanSDK/1.0.3.1/Include")
    set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} "C:/libs/VulkanSDK/1.0.3.1/Bin32")
    find_library(VULKAN_LIBS NAMES vulkan-1 PATHS ${CMAKE_LIBRARY_PATH})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVK_USE_PLATFORM_WIN32_KHR")
ELSE(WIN32)
    message("Building for Linux")
    set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} "/usr/lib64")
    find_library(VULKAN_LIBS NAMES libvulkan.so PATHS ${CMAKE_LIBRARY_PATH})
    find_package(XCB REQUIRED)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVK_USE_PLATFORM_XCB_KHR -std=c++11 -Wall -Wextra -g -O3")
ENDIF(WIN32)
set(LIBS ${LIBS} ${VULKAN_LIBS})

set(INCLUDE_DIR ./ "C:/libs/VulkanSDK/1.0.3.1/Include")
include_directories(${INCLUDE_DIR})

# Find all the source files
set(SOURCE_FILES vulkanMain.cpp vulkanHelpers.cpp vulkanHelpers.h VulkanContext.cpp VulkanContext.h vkbpDefines.h vkbpDefines.cpp vkbp.h)

# Make an executable called DualityEngine out of all the above source files
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

# Link to external libraries that were found above
target_link_libraries(${PROJECT_NAME} ${LIBS})