# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
add_executable(acord
    main.cpp
    WsSession.hpp
    WsSession.cpp
    LocalSession.hpp
    LocalSession.cpp
)

target_link_libraries(acord PRIVATE
    acord::common
    acord::asset-mgr
    ac::local
    ac::jalog
)

if(TARGET aclp::out-dir)
    target_link_libraries(acord PRIVATE
        aclp::out-dir
    )
endif()
