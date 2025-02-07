# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
ac_dep(furi)
ac_dep(boost)

icm_add_lib(acord-server ACORD_SERVER)
add_library(acord::server ALIAS acord-server)

target_sources(acord-server PRIVATE
    acord/server/api.h
    acord/server/Logging.hpp
    acord/server/Logging.cpp
    acord/server/FsUtil.hpp
    acord/server/FsUtil.cpp
    acord/server/AssetMgr.hpp
    acord/server/AssetMgr.cpp
)

target_include_directories(acord-server INTERFACE .)

target_link_libraries(acord-server
    PUBLIC
        acord::common    
    PRIVATE
        ac::jalog
        acord::http
        Boost::asio
)
