// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "api.h"
#include <fishnets/WebSocketSessionPtr.hpp>

namespace acord::client {
ACORD_CLIENT_API fishnets::WebSocketSessionPtr makeWsSession();
} // namespace acord::server
