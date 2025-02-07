// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "App.hpp"
#include <thread>

namespace acord::server {

class AppRunner {
    App m_app;
    std::thread m_thread;
public:
    AppRunner(uint16_t wsPort = Default_WsPort)
        : m_app(wsPort)
    {
        m_thread = std::thread([&] {
            m_app.run();
        });
    }

    ~AppRunner() {
        m_app.stop();
        m_thread.join();
    }
};

} // namespace acord::server
