# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
icm_add_lib(acord-client ACORD_COMMON)
add_library(acord::client ALIAS acord-client)

target_sources(acord-client
    INTERFACE FILE_SET HEADERS FILES
        acord/client/api.h
        acord/client/WsSession.hpp
    PRIVATE
        acord/client/WsSession.cpp
)

target_link_libraries(acord-client
    PUBLIC
        acord::common
    PRIVATE
        ac::jalog
)
