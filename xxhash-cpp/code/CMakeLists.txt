# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
ac_dep(xxhash)

icm_add_lib(xxhash-cpp XXHASH_CPP
    xxhash/export.h
    xxhash/xxh-config.h
    xxhash/hash.hpp
    xxhash/hash.cpp
)
add_library(ac::xxhash-cpp ALIAS xxhash-cpp)
target_link_libraries(xxhash-cpp PUBLIC
    splat::splat
    xxhash::xxhash
)
target_include_directories(xxhash-cpp INTERFACE .)
