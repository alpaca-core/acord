// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "App.hpp"

#include "WsSession.hpp"
#include "LocalSession.hpp"
#include "AppCtx.hpp"

#include "AssetMgr.hpp"

#include <ac/local/Lib.hpp>

#include <ac/jalog/Instance.hpp>
#include <ac/jalog/sinks/DefaultSink.hpp>

#include <ac/frameio/local/LocalIoCtx.hpp>

#include <fishnets/WebSocketServer.hpp>

#ifdef HAVE_ACLP_OUT_DIR
#include "aclp-out-dir.h"
#endif

namespace acord::server {

struct App::Impl {
    ac::frameio::LocalIoCtx io;
    AssetMgr assetMgr;
    LocalSessionFactory sessionFactory{ assetMgr };
    AppCtx ctx{io, sessionFactory};
    fishnets::WebSocketServer server;

    Impl(uint16_t wsPort)
        : server([&](const fishnets::WebSocketEndpointInfo&) {
            return makeWsSession(ctx);
        }, wsPort, 3, nullptr)
    {
#ifdef HAVE_ACLP_OUT_DIR
        ac::local::Lib::addPluginDir(ACLP_OUT_DIR);
#endif
        ac::local::Lib::loadAllPlugins();
    }
};

App::App(uint16_t wsPort)
    : m_impl(std::make_unique<Impl>(wsPort))
{}

App::~App() = default;

void App::run() {
    m_impl->io.run();
}

void App::stop() {
    m_impl->io.forceStop();
}

} // namespace acord::server
