
# Galen Cochrane
# Copyright (C) 2016
# This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)

# No lesser version of cMake may be used.
cmake_minimum_required(VERSION 3.3)

# Assign a project name
project(VKBP)

option(GLFW_BUILD_EXAMPLES "Build the GLFW example programs" OFF)
option(GLFW_BUILD_TESTS "Build the GLFW test programs" OFF)
option(GLFW_BUILD_DOCS "Build the GLFW documentation" OFF)
option(GLFW_INSTALL "Generate installation target" OFF)
option(GLFW_USE_CHDIR "Make glfwInit chdir to Contents/Resources" OFF)
option(GLFW_USE_MENUBAR "Populate the menu bar on first window creation" OFF)
add_subdirectory(glfw)
set(INCLUDE_DIRS ${INCLUDE_DIRS} glfw/include)
set(LIBS ${LIBS} glfw ${GLFW_LIBRARIES})

# Examine and build vkbp directories
add_subdirectory( vkbp )

# Add the test files to sources
set( INCLUDE_DIRS ${INCLUDE_DIRS} ${CMAKE_CURRENT_LIST_DIR} )
set( SOURCE_FILES ${SOURCE_FILES} vkbpTest.cpp )

# Build
include_directories( ${INCLUDE_DIRS} )
add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} ${LIBS})