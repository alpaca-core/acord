# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
add_subdirectory(code)
if(ACORD_BUILD_TESTS)
    add_subdirectory(test)
endif()
