
# Galen Cochrane
# Copyright (C) 2016
# This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)

# set DIR to point to current directory (relative to top level)
set( DIR ${CMAKE_CURRENT_LIST_DIR} )

# depending on OS, find Vulkan and windowing libraries and set appropriate flags
IF( WIN32 OR CYGWIN )
    message( "Building vkbp for Windows" )
    set( VULKAN_ROOT $ENV{VULKAN_SDK} )
    set( LIBS ${LIBS} "${VULKAN_ROOT}/Bin32/vulkan-1.lib" PARENT_SCOPE )
    set( INCLUDE_DIRS ${INCLUDE_DIRS} "${VULKAN_ROOT}/Include" PARENT_SCOPE )
ELSE(  )
    message( "Building vkbp for Linux" )
    set( CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} "/usr/lib64" PARENT_SCOPE )
    find_library( VULKAN_LIBS NAMES libvulkan.so PATHS ${CMAKE_LIBRARY_PATH} )
    set( LIBS ${LIBS} ${VULKAN_LIBS} ${XCB_LIBRARY} PARENT_SCOPE )
ENDIF(  )

add_subdirectory( source )

# include the current directory
set( INCLUDE_DIRS ${INCLUDE_DIRS} ${DIR} PARENT_SCOPE )

# include all headers/source in current directory
set( SOURCE_FILES ${SOURCE_FILES}
        "${DIR}/vkbp.h"
        PARENT_SCOPE
        )
