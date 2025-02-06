# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
ac_dep(furi)
ac_dep(boost)

icm_add_lib(acord-asset-mgr ACORD_ASSET_MGR)
add_library(acord::asset-mgr ALIAS acord-asset-mgr)

target_sources(acord-asset-mgr PRIVATE
    acord/asset/api.h
    acord/asset/Logging.hpp
    acord/asset/Logging.cpp
    acord/asset/FsUtil.hpp
    acord/asset/FsUtil.cpp
    acord/asset/Manager.hpp
    acord/asset/Manager.cpp
)

target_include_directories(acord-asset-mgr INTERFACE .)

target_link_libraries(acord-asset-mgr
    PUBLIC
        ac::frame
    PRIVATE
        ac::jalog
        acord::http
        Boost::asio
)
