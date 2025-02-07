// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "api.h"
#include <vector>
#include <string>
#include <memory>

namespace ac::frameio {
struct StreamEndpoint;
}

namespace acord::server {

class ACORD_SERVER_API AssetMgr {
public:
    AssetMgr();
    ~AssetMgr();

    AssetMgr(const AssetMgr&) = delete;
    AssetMgr& operator=(const AssetMgr&) = delete;

    ac::frameio::StreamEndpoint makeAssetsAvailable(std::vector<std::string> assetUrls);
private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

} // namespace acord::server
