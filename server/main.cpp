// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "WsSession.hpp"

#include <ac/local/Lib.hpp>
#include <ac/frameio/local/LocalIoCtx.hpp>

#include <ac/jalog/Instance.hpp>
#include <ac/jalog/sinks/DefaultSink.hpp>

#include <fishnets/WebSocketServer.hpp>

#ifdef HAVE_ACLP_OUT_DIR
#include "aclp-out-dir.h"
#endif

int main() {
    ac::jalog::Instance jl;
    jl.setup().add<ac::jalog::sinks::DefaultSink>();

#ifdef HAVE_ACLP_OUT_DIR
    ac::local::Lib::addPluginDir(ACLP_OUT_DIR);
#endif

    ac::local::Lib::loadAllPlugins();

    fishnets::WebSocketServer server(makeWsSession, 7654, 3, nullptr);

    ac::frameio::LocalIoCtx io;
    io.run();

    // should never get to here
    return 0;
}