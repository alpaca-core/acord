# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#

# cpm
if(NOT CPM_SOURCE_CACHE AND NOT DEFINED ENV{CPM_SOURCE_CACHE})
    set(CPM_SOURCE_CACHE "${CMAKE_SOURCE_DIR}/.cpm" CACHE PATH "CPM source cache")
    message(STATUS "Setting cpm cache dir to: ${CPM_SOURCE_CACHE}")
endif()
include(./get_cpm.cmake)

# ac-build
if(AC_BUILD_MONO)
    set(acLocalCMakeLibDir "${CMAKE_SOURCE_DIR}/../ac-build")
else()
    CPMAddPackage(gh:alpaca-core/ac-build@1.0.0)
    set(acLocalCMakeLibDir "${ac-build_SOURCE_DIR}")
endif()

list(APPEND CMAKE_MODULE_PATH
    "${acLocalCMakeLibDir}"
)
