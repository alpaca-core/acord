// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "WsSession.hpp"
#include "LocalSession.hpp"
#include "AppCtx.hpp"

#include <acord/asset/Manager.hpp>

#include <ac/local/Lib.hpp>
#include <ac/frameio/local/LocalIoCtx.hpp>

#include <ac/jalog/Instance.hpp>
#include <ac/jalog/sinks/DefaultSink.hpp>

#include <fishnets/WebSocketServer.hpp>

#ifdef HAVE_ACLP_OUT_DIR
#include "aclp-out-dir.h"
#endif

using namespace acord::server;

int main() {
    ac::jalog::Instance jl;
    jl.setup().add<ac::jalog::sinks::DefaultSink>();

#ifdef HAVE_ACLP_OUT_DIR
    ac::local::Lib::addPluginDir(ACLP_OUT_DIR);
#endif

    ac::local::Lib::loadAllPlugins();

    ac::frameio::LocalIoCtx io;
    acord::asset::Manager assetMgr;
    LocalSessionFactory sessionFactory{assetMgr};
    AppCtx ctx{io, sessionFactory};

    fishnets::WebSocketServer server([&](const fishnets::WebSocketEndpointInfo&) {
        return makeWsSession(ctx);
    }, 7654, 3, nullptr);

    io.run();

    // should never get to here
    return 0;
}