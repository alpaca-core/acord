// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "Connection.hpp"

#include <acord/CommonWsSession.hpp>
#include <fishnets/WebSocketClient.hpp>

namespace acord::client {

namespace {
class WsSession final : public CommonWsSession {
public:
    WsSession(ac::frameio::StreamEndpoint ep) {
        m_dispatch = std::move(ep);
    }
};
} // namespace

void Connection_initiate(std::string host, uint16_t port, ac::frameio::StreamEndpoint localEndpoint) {
    fishnets::WebSocketClient client([&](const fishnets::WebSocketEndpointInfo&) -> fishnets::WebSocketSessionPtr {
        return std::make_shared<WsSession>(std::move(localEndpoint));
    });
    client.connect(host, port);
}

} // namespace acord::client