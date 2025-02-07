// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "api.h"
#include "DefaultPort.hpp"
#include <ac/frameio/local/LocalIoCtx.hpp>

namespace acord::server {

struct ACORD_SERVER_API App {
    ac::frameio::LocalIoCtx m_ioCtx;

    // block current thread
    void run(uint16_t wsPort = Default_WsPort);

    // safe on any thread
    void stop();
};

} // namespace acord::server
