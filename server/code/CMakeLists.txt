# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
ac_dep(furi)
ac_dep(boost)
ac_dep(fishnets)

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

    acord/server/LocalSession.hpp
    acord/server/LocalSession.cpp

    acord/server/WsSession.hpp
    acord/server/WsSession.cpp

    acord/server/App.hpp
    acord/server/App.cpp
)

target_include_directories(acord-server INTERFACE .)

target_link_libraries(acord-server
    PUBLIC
        acord::common
        ac::local
        fishnets::fishnets
    PRIVATE
        ac::jalog
        acord::http
        Boost::asio
)

if(TARGET aclp::out-dir)
    target_link_libraries(acord-server PRIVATE
        aclp::out-dir
    )
endif()