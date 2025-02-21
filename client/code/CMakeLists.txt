# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
icm_add_lib(acord-client ACORD_CLIENT)
add_library(acord::client ALIAS acord-client)

target_sources(acord-client
    INTERFACE FILE_SET HEADERS FILES
        acord/client/api.h
        acord/client/IoCtx.hpp
    PRIVATE
        acord/client/Logging.hpp
        acord/client/Logging.cpp
        acord/client/IoCtx.cpp
)

target_link_libraries(acord-client
    PUBLIC
        ac::frame
    PRIVATE
        acord::common
        ac::jalog
)
