// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "App.hpp"

#include "WsSession.hpp"
#include "LocalSession.hpp"
#include "AppCtx.hpp"

#include "AssetMgr.hpp"

#include <ac/local/Lib.hpp>

#include <ac/jalog/Instance.hpp>
#include <ac/jalog/sinks/DefaultSink.hpp>

#include <ac/frameio/local/LocalIoCtx.hpp>

#include <fishnets/Context.hpp>
#include <fishnets/WsServerHandler.hpp>
#include <fishnets/util/WsSessionHandler.hpp>

#include <thread>

#ifdef HAVE_ACLP_OUT_DIR
#include "aclp-out-dir.h"
#endif

namespace acord::server {

struct App::Impl {
    ac::frameio::LocalIoCtx io;
    AssetMgr assetMgr;
    LocalSessionFactory sessionFactory{assetMgr};
    AppCtx ctx{io, sessionFactory};
    fishnets::Context wsCtx;
    std::vector<std::thread> wsThreads;

    Impl(uint16_t wsPort)
    {
#ifdef HAVE_ACLP_OUT_DIR
        ac::local::Lib::addPluginDir(ACLP_OUT_DIR);
#endif
        ac::local::Lib::loadAllPlugins();

        wsCtx.wsServeLocalhost(wsPort, std::make_shared<fishnets::SimpleServerHandler>([this](const fishnets::EndpointInfo&, const fishnets::EndpointInfo&) {
            return makeWsSession(ctx);
        }));
        for (int i = 0; i < 3; ++i) {
            wsThreads.emplace_back([this] {
                wsCtx.run();
            });
        }
    }

    ~Impl() {
        wsCtx.stop();
        for (auto& t : wsThreads) {
            t.join();
        }
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
