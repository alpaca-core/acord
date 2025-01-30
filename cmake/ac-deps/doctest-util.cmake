# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
if(NOT TARGET doctest::util)
    CPMAddPackage(gh:iboB/doctest-util@0.1.3)
    set_target_properties(doctest PROPERTIES FOLDER test)
    set_target_properties(doctest-main PROPERTIES FOLDER test)
    enable_testing()
endif()
