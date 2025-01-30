// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <fishnets/WebSocketSessionPtr.hpp>

namespace acord::server {
struct AppCtx;
fishnets::WebSocketSessionPtr makeWsSession(const AppCtx& ctx);
} // namespace acord::server
