
# Galen Cochrane
# Copyright (C) 2016
# This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)

# No lesser version of cMake may be used.
cmake_minimum_required( VERSION 3.3 )

# Assign a project name
project( VKBP )

if( NOT WIN32 )
    # these are the compiler flags to be used on gcc
    set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS} -ggdb3 -Wall -std=c++11")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -Ofast -Wall -std=c++11")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO  "${CMAKE_CXX_FLAGS} -Og -ggdb3 -Wall -std=c++11")
endif()

option( USE_INCLUDED_GLFW "Build GLFW from included source." OFF )

if( USE_INCLUDED_GLFW )
    message( "Using included GLFW source." )
    option( GLFW_BUILD_EXAMPLES "Build the GLFW example programs" OFF )
    option( GLFW_BUILD_TESTS "Build the GLFW test programs" OFF )
    option( GLFW_BUILD_DOCS "Build the GLFW documentation" OFF )
    option( GLFW_INSTALL "Generate installation target" OFF )
    option( GLFW_USE_CHDIR "Make glfwInit chdir to Contents/Resources" OFF )
    option( GLFW_USE_MENUBAR "Populate the menu bar on first window creation" OFF )
    add_subdirectory( glfw )
    set( INCLUDE_DIRS ${INCLUDE_DIRS} glfw/include )
    set( LIBS ${LIBS} glfw ${GLFW_LIBRARIES} )
else( )
    message( "Using system provided GLFW." )
    find_package( PkgConfig REQUIRED )
    pkg_search_module( GLFW REQUIRED glfw3 )
    set( INCLUDE_DIRS ${INCLUDE_DIRS} ${GLFW_INCLUDE_DIRS} )
    set( LIBS ${LIBS} ${GLFW_LIBRARIES} )
endif( )

# Examine and build vkbp directories
add_subdirectory( vkbp )

# Add the test files to sources
set( INCLUDE_DIRS ${INCLUDE_DIRS} ${CMAKE_CURRENT_LIST_DIR} )
set( SOURCE_FILES ${SOURCE_FILES} vkbpTest.cpp )

# Build
include_directories( ${INCLUDE_DIRS} )
add_executable( ${PROJECT_NAME} ${SOURCE_FILES} )
target_link_libraries( ${PROJECT_NAME} ${LIBS} )