// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "api.h"
#include <acord/DefaultPort.hpp>
#include <memory>

namespace acord::server {

class ACORD_SERVER_API App {
public:
    App(uint16_t wsPort = Default_WsPort);
    ~App();

    App(const App&) = delete;
    App& operator=(const App&) = delete;

    // block current thread
    void run();

    // safe on any thread
    void stop();
private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

} // namespace acord::server
