# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
add_library(acord-client INTERFACE)
add_library(acord::client ALIAS acord-client)
target_link_libraries(acord-client INTERFACE acord::common)
target_include_directories(acord-client INTERFACE .)
