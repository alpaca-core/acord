# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
icm_add_lib(acord-common ACORD_COMMON)
add_library(acord::common ALIAS acord-common)

target_sources(acord-common
    INTERFACE FILE_SET HEADERS FILES
        acord/api.h
        acord/Frame.hpp
        acord/FrameCvt.hpp
        acord/CommonWsSession.hpp
    PRIVATE
        acord/CommonWsSession.cpp
)

target_link_libraries(acord-common
    PUBLIC
        ac::frame
        fishnets::fishnets
    PRIVATE
        ac::jalog
)
