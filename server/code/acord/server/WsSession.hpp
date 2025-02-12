// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "api.h"
#include <fishnets/util/WsSessionHandlerPtr.hpp>

namespace acord::server {
struct AppCtx;
[[nodiscard]] ACORD_SERVER_API fishnets::WsSessionHandlerPtr makeWsSession(const AppCtx& ctx);
} // namespace acord::server
