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
    AppRunner(AppParams params = {})
        : m_app(std::move(params))
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
