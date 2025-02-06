# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
cmake_minimum_required(VERSION 3.24 FATAL_ERROR)

project(acord
    VERSION 0.1.0
    DESCRIPTION "Alpaca Core local daemon"
    LANGUAGES CXX
)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(./get-ac-build.cmake)

include(init_ac_prj)
include(ac_build_prj_util)

option(ACORD_BUILD_TESTS "${PROJECT_NAME}: build tests" ${testsDefault})
option(ACORD_BUILD_EXAMPLES "${PROJECT_NAME}: build examples" ${examplesDefault})
mark_as_advanced(ACORD_BUILD_TESTS ACORD_BUILD_EXAMPLES)

macro(acord_add_test_subdir)
    if(ACORD_BUILD_TESTS)
        add_subdirectory(test)
    endif()
endmacro()

#######################################
# packages

add_ac_local(0.1.11)

if(ACORD_BUILD_TESTS)
    ac_dep(doctest-util)
endif()

ac_dep(fishnets)

#######################################
# plugins

if(NOT AC_BUILD_DEPLOY)
    make_ac_local_plugin_available(
        NAME ilib-foo
        VERSION 1.0.0
        GITHUB alpaca-core/ilib-foo
    )
endif()

#######################################
# subdirs / targets
add_subdirectory(common)
add_subdirectory(server)
add_subdirectory(client)
if(ACORD_BUILD_EXAMPLES)
    add_subdirectory(example)
endif()
