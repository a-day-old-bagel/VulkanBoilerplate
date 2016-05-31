
# Galen Cochrane
# Copyright (C) 2016
# This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)

# set DIR to point to current directory (relative to top level)
set( DIR ${CMAKE_CURRENT_LIST_DIR} )

# include the current directory
set( INCLUDE_DIRS ${INCLUDE_DIRS} ${DIR} PARENT_SCOPE )

# include all headers/source in the current directory
set( SOURCE_FILES ${SOURCE_FILES}
        "${DIR}/Vkbp.cpp"           "${DIR}/Vkbp.h"
        "${DIR}/vkbpGlobal.cpp"     "${DIR}/vkbpGlobal.h"
        "${DIR}/vkbpHelpers.cpp"    "${DIR}/vkbpHelpers.h"
        "${DIR}/VkbpWindow.cpp"     "${DIR}/VkbpWindow.h"
        PARENT_SCOPE
        )