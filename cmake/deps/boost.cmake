# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
if(TARGET Boost::boost)
    # boost came from a parent project
    return()
endif()

# add what we need (child projects may suffer as this is a trimmed-down version)
CPMAddPackage(gh:iboB/boost-trim@1.85.0)
add_library(Boost::asio ALIAS Boost::boost)
add_library(Boost::beast ALIAS Boost::boost)
