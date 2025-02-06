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

namespace acord::asset {

class ACORD_ASSET_MGR_API Manager {
public:
    Manager();
    ~Manager();

    Manager(const Manager&) = delete;
    Manager& operator=(const Manager&) = delete;

    ac::frameio::StreamEndpoint makeAssetsAvailable(std::vector<std::string> assetUrls);
private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

} // namespace acord::asset
