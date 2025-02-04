# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
set(tgt acord-example-hello-foo)
add_executable(${tgt} e-hello-foo.cpp)
target_link_libraries(${tgt} PRIVATE
    acord::client
    ac::jalog
)
