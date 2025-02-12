// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "api.h"
#include <acord/DefaultPort.hpp>
#include <ac/frameio/StreamEndpoint.hpp>
#include <string>
#include <cstdint>

namespace acord::client {

class ACORD_CLIENT_API IoCtx {
public:
    IoCtx(size_t numThreads = 1);
    ~IoCtx();
    IoCtx(const IoCtx&) = delete;
    IoCtx& operator=(const IoCtx&) = delete;

    void connect(
        ac::frameio::StreamEndpoint localEndpoint,
        uint16_t port = Default_WsPort
    );
private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

} // namespace acord::client
