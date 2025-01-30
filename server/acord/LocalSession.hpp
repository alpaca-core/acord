// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <ac/frameio/SessionHandlerPtr.hpp>

namespace acord::server {
struct LocalSessionFactory {
    ac::frameio::SessionHandlerPtr createHandler();
};
} // namespace acord::server
