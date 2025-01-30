# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
macro(add_common_test test)
    add_doctest_lib_test(${test} acord-common t-${test}.cpp)
endmacro()

add_common_test(Frame)
add_common_test(FrameCvt)
