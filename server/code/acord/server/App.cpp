// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "App.hpp"

#include "WsSession.hpp"
#include "LocalSession.hpp"
#include "AppCtx.hpp"

#include "AssetMgrService.hpp"

#include <ac/local/Lib.hpp>

#include <ac/jalog/Instance.hpp>
#include <ac/jalog/sinks/DefaultSink.hpp>

#include <ac/local/DefaultBackend.hpp>

#include <fishnets/Context.hpp>
#include <fishnets/WsServerHandler.hpp>
#include <fishnets/util/WsSessionHandler.hpp>

#include <ac/xec/context.hpp>
#include <ac/xec/context_work_guard.hpp>

#include <thread>

#ifdef HAVE_ACLP_OUT_DIR
#include "aclp-out-dir.h"
#endif

namespace acord::server {

struct App::Impl {
    ac::local::DefaultBackend backend;
    ac::xec::context localSessionXCtx;
    ac::xec::context_work_guard wg;
    AppCtx ctx{localSessionXCtx, backend};
    fishnets::Context wsCtx;
    std::vector<std::thread> wsThreads;

    Impl(AppParams params)
        : wg(localSessionXCtx)
    {
#ifdef HAVE_ACLP_OUT_DIR
        ac::local::Lib::addPluginDir(ACLP_OUT_DIR);
#endif
        ac::local::Lib::loadAllPlugins();

        backend.registerLibServices();
        backend.registerService(AssetMgr_getServiceFactory());

        std::vector<fishnets::EndpointInfo> endpoints;
        if (params.serveLocalhostOnly) {
            endpoints.push_back({"127.0.0.1", params.wsPort});
            endpoints.push_back({"::1", params.wsPort});
        }
        else{
            endpoints.push_back({fishnets::IPv4, params.wsPort});
            // TODO: Check why when IPv6 is enabled, the server does not start
            // when it's started in docker
            // endpoints.push_back({fishnets::IPv6, params.wsPort});
        }

        wsCtx.wsServe(endpoints, std::make_shared<fishnets::SimpleServerHandler>([this](const fishnets::EndpointInfo&, const fishnets::EndpointInfo&) {
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

App::App(AppParams params)
    : m_impl(std::make_unique<Impl>(std::move(params)))
{}

App::~App() = default;

void App::run() {
    m_impl->localSessionXCtx.run();
}

void App::stop() {
    m_impl->localSessionXCtx.stop();
}

} // namespace acord::server
