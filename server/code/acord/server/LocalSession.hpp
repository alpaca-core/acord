// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <ac/frameio/SessionHandlerPtr.hpp>
namespace acord::server {
class AssetMgr;
struct LocalSessionFactory {
    AssetMgr& assetMgr;
    ac::frameio::SessionHandlerPtr createHandler();
};
} // namespace acord::server
