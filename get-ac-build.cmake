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
    set(ac-build_SOURCE_DIR "${CMAKE_SOURCE_DIR}/../ac-build")
else()
    CPMAddPackage(gh:alpaca-core/ac-build@1.1.1)
endif()

list(APPEND CMAKE_MODULE_PATH
    "${ac-build_SOURCE_DIR}"
)
