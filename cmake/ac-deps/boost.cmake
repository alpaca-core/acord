# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
if(NOT TARGET Boost::boost)
    # only add boost if not added from the outside
    CPMAddPackage(gh:iboB/boost-trim@1.85.0)
endif()

if(NOT TARGET Boost::asio)
    # asio is header only, so this is fine
    add_library(Boost::asio ALIAS Boost::boost)
endif()

if(NOT TARGET Boost::beast)
    # beast is header only, so this is fine
    add_library(Boost::beast ALIAS Boost::boost)
endif()
