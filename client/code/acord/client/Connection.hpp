// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "api.h"
#include <ac/frameio/StreamEndpoint.hpp>
#include <string>
#include <cstdint>

namespace acord::client {

class ACORD_CLIENT_API Connection {
public:
    Connection();
    ~Connection();
    Connection(const Connection&) = delete;
    Connection& operator=(const Connection&) = delete;

    void initiate(
        std::string host, uint16_t port,
        ac::frameio::StreamEndpoint localEndpoint
    );

    void close();

private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

} // namespace acord::client
