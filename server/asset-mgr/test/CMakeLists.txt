# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
configure_file(test-assets.in.h test-assets.h @ONLY)
add_library(acord-amgr-test-assets INTERFACE)
target_include_directories(acord-amgr-test-assets INTERFACE ${CMAKE_CURRENT_BINARY_DIR})

macro(add_amgr_test test)
    add_doctest_lib_test(${test} acord-asset-mgr
        t-${test}.cpp
        LIBRARIES
            acord-amgr-test-assets
            ac::jalog
    )
endmacro()

add_amgr_test(FsUtil)
add_amgr_test(Manager)
