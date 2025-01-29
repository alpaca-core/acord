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

#######################################
# packages

add_ac_local(0.1.9)
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

add_subdirectory(server)
