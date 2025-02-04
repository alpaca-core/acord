// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "api.h"
#include <ac/frameio/StreamEndpoint.hpp>
#include <string>
#include <cstdint>

namespace acord::client {

ACORD_CLIENT_API void Connection_initiate(
    std::string host, uint16_t port,
    ac::frameio::StreamEndpoint localEndpoint
);

} // namespace acord::client
