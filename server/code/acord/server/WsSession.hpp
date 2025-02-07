// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "api.h"
#include <fishnets/WebSocketSessionPtr.hpp>

namespace acord::server {
struct AppCtx;
[[nodiscard]] ACORD_SERVER_API fishnets::WebSocketSessionPtr makeWsSession(const AppCtx& ctx);
} // namespace acord::server
