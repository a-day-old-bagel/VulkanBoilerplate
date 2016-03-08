# No lesser version of cMake may be used.
cmake_minimum_required(VERSION 3.3)

# Declare the project in question
project(Vulkan)

# Make sure the project compiles with the c++11 and thread safety flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra -g -O3")

# Tell cMake where to look for the modules that search for SDL2 (the others come with cMake)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

# Tell cMake where to look for third party libs (add to this for your own builds)
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} "C:/libs/VulkanSDK/1.0.3.1/Include/vulkan")
set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} "C:/libs/VulkanSDK/1.0.3.1/Bin")

set(INCLUDE_DIR ./ "C:/libs/VulkanSDK/1.0.3.1/Include/vulkan")
include_directories(${INCLUDE_DIR})

# Find all the source files
set(SOURCE_FILES vulkanMain.cpp)

# Make an executable called DualityEngine out of all the above source files
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

# Link to external libraries that were found above
target_link_libraries(${PROJECT_NAME} ${LIBS})