// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <ac/frameio/SessionHandlerPtr.hpp>

namespace acord::asset {
class Manager;
}

namespace acord::server {
struct LocalSessionFactory {
    asset::Manager& assetMgr;
    ac::frameio::SessionHandlerPtr createHandler();
};
} // namespace acord::server
