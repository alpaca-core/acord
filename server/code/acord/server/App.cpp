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

#include <fishnets/WebSocketServer.hpp>

#ifdef HAVE_ACLP_OUT_DIR
#include "aclp-out-dir.h"
#endif

namespace acord::server {

void App::run(uint16_t wsPort) {
#ifdef HAVE_ACLP_OUT_DIR
    ac::local::Lib::addPluginDir(ACLP_OUT_DIR);
#endif

    ac::local::Lib::loadAllPlugins();

    ac::frameio::LocalIoCtx io;
    AssetMgr assetMgr;
    LocalSessionFactory sessionFactory{ assetMgr };
    AppCtx ctx{io, sessionFactory};

    fishnets::WebSocketServer server([&](const fishnets::WebSocketEndpointInfo&) {
        return makeWsSession(ctx);
    }, wsPort, 3, nullptr);

    io.run();
}

void App::stop() {
    m_ioCtx.forceStop();
}

} // namespace acord::server
