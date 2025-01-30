# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
add_library(acord-common INTERFACE)
add_library(acord::common ALIAS acord-common)
target_link_libraries(acord-common INTERFACE ac::frame)
target_include_directories(acord-common INTERFACE .)
