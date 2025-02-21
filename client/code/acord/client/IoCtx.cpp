// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "IoCtx.hpp"
#include "Logging.hpp"

#include <acord/CommonWsSession.hpp>
#include <fishnets/Context.hpp>
#include <fishnets/ContextWorkGuard.hpp>
#include <astl/multi_thread_runner.hpp>
#include <astl/move_capture.hpp>
#include <astl/make_ptr.hpp>
#include <thread>

namespace acord::client {

namespace {
class WsSession final : public CommonWsSession {
public:
    WsSession(ac::frameio::StreamEndpoint ep) {
        m_dispatch = std::move(ep);
    }
    virtual void wsOpened(std::string_view target) override {
        ACORD_CLIENT_LOG(Info, "Session opened: ", target);
        CommonWsSession::wsOpened(target);
        tryReadFromDispatch();
    }
    void wsClosed(std::string reason) override {
        ACORD_CLIENT_LOG(Info, "Session closed: ", reason);
        CommonWsSession::wsClosed(std::move(reason));
    }
};
} // namespace

struct IoCtx::Impl {
    fishnets::Context m_ctx;
    fishnets::ContextWorkGuard m_workGuard = m_ctx.makeWorkGuard();
    astl::multi_thread_runner m_threads;

    Impl(size_t numThreads)
        : m_threads(m_ctx, numThreads)
    {}

    ~Impl() {
        m_workGuard.reset();
    }
};

IoCtx::IoCtx(size_t numThreads)
    : m_impl(std::make_unique<Impl>(numThreads))
{}

IoCtx::~IoCtx() = default;

void IoCtx::connect(
    ac::frameio::StreamEndpoint localEndpoint,
    uint16_t port
) {
    auto session = std::make_shared<WsSession>(std::move(localEndpoint));
    m_impl->m_ctx.wsConnect(std::move(session), fishnets::EndpointInfo{"127.0.0.1", port});
}

} // namespace acord::client