# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
add_executable(acord
    main.cpp
    WsSession.hpp
    WsSession.cpp
)

target_link_libraries(acord PRIVATE
    ac::local
    ac::jalog
    fishnets::fishnets
)

if(TARGET aclp::out-dir)
    target_link_libraries(acord PRIVATE
        aclp::out-dir
    )
endif()
