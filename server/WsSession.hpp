// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <fishnets/WebSocketSessionPtr.hpp>

struct AppCtx;
fishnets::WebSocketSessionPtr makeWsSession(const AppCtx& ctx);
