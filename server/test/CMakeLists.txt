# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
configure_file(test-assets.in.h test-assets.h @ONLY)
add_library(acord-test-assets INTERFACE)
target_include_directories(acord-test-assets INTERFACE ${CMAKE_CURRENT_BINARY_DIR})

macro(add_server_test test)
    add_doctest_lib_test(${test} acord-server
        t-${test}.cpp
        ${ARGN}
    )
endmacro()

add_server_test(FsUtil
    LIBRARIES acord-test-assets
)
add_server_test(AssetMgr 
    LIBRARIES
        acord-test-assets
        ac::jalog
)
