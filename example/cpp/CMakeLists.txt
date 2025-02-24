# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
option(ACORD_EXAMPLES_SAME_PROCESS_SERVER "${PROJECT_NAME}-examples: run the server in the same process as the app" OFF)

set(tgt acord-example-hello-foo)
add_executable(${tgt} e-hello-foo.cpp)
target_link_libraries(${tgt} PRIVATE
    acord::client
    acord::server
    ac::jalog
)
if(ACORD_EXAMPLES_SAME_PROCESS_SERVER)
    target_compile_definitions(${tgt} PRIVATE SAME_PROCESS_SERVER=1)
endif()
