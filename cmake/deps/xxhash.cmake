# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
include_guard(GLOBAL)
CPMAddPackage(gh:Cyan4973/xxHash@0.8.2)
add_library(xxhash INTERFACE)
add_library(xxhash::xxhash ALIAS xxhash)
target_include_directories(xxhash INTERFACE ${xxHash_SOURCE_DIR})
