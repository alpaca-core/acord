// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <ac/frameio/StreamEndpoint.hpp>

namespace acord::server {
struct AppCtx;
void LocalSession_connect(const AppCtx& appCtx, ac::frameio::StreamEndpoint ep);
} // namespace acord::server
