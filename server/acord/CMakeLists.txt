# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
add_executable(acord
    main.cpp
)

target_link_libraries(acord PRIVATE
    acord::server
    ac::jalog
)
