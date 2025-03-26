# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
cmake_minimum_required(VERSION 3.24 FATAL_ERROR)

project(acord
    VERSION 0.1.1
    DESCRIPTION "Alpaca Core local daemon"
    LANGUAGES CXX
)

include(./get-ac-build.cmake)

CPMAddPackage(gh:iboB/icm@1.5.4)
list(APPEND CMAKE_MODULE_PATH
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake"
    "${icm_SOURCE_DIR}"
)

include(init_ac_prj)
include(ac_build_prj_util)
include(icm_add_lib)

option(BUILD_SHARED_LIBS "Build shared libraries" ON) # not really needed, but it's a workaround for FindOpenSSL.cmake
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

add_ac_local(0.1.20)

if(ACORD_BUILD_TESTS)
    ac_dep(doctest-util)
endif()

#######################################
# plugins

if(NOT AC_BUILD_DEPLOY)
    make_ac_local_plugin_available(
        NAME ilib-foo
        VERSION 0.1.3
        GITHUB alpaca-core/ilib-foo
    )

    make_ac_local_plugin_available(
        NAME ilib-llama.cpp
        VERSION 0.1.1
        GITHUB alpaca-core/ilib-llama.cpp
    )

    make_ac_local_plugin_available(
        NAME ilib-sd.cpp
        VERSION 0.1.1
        GITHUB alpaca-core/ilib-sd.cpp
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
