// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "Connection.hpp"

#include <acord/CommonWsSession.hpp>
#include <fishnets/WebSocketClient.hpp>
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
    virtual void wsOpened() override {
        tryReadFromDispatch();
    }
};
} // namespace

Connection::Connection() = default;
Connection::~Connection() {
    close();
}

struct Connection::Impl {
    Impl(ac::frameio::StreamEndpoint ep)
        : m_localEndpoint(std::move(ep))
        , m_client([this](const fishnets::WebSocketEndpointInfo&) mutable -> fishnets::WebSocketSessionPtr {
            return std::make_shared<WsSession>(std::move(m_localEndpoint));
        })
    {}

    ac::frameio::StreamEndpoint m_localEndpoint;
    fishnets::WebSocketClient m_client;
    std::thread m_thread;
};

void Connection::initiate(std::string host, uint16_t port, ac::frameio::StreamEndpoint localEndpoint) {
    m_impl = std::make_unique<Impl>(std::move(localEndpoint));
    m_impl->m_thread = std::thread([this, movecap(host, port)]() {
        m_impl->m_client.connect(host, port);
    });
}

void Connection::close() {
    if (!m_impl) return;
    m_impl->m_client.stop();
    if (m_impl->m_thread.joinable()) {
        m_impl->m_thread.join();
    }
    m_impl.reset();
}

} // namespace acord::client