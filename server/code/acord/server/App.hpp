// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "api.h"
#include <acord/DefaultPort.hpp>
#include <memory>

namespace acord::server {
struct AppParams {
    uint16_t wsPort = Default_WsPort;
    bool serveLocalhostOnly = true;
};
class ACORD_SERVER_API App {
public:


    App(AppParams params = {});
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
