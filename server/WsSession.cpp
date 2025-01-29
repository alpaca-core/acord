// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "WsSession.hpp"
#include <fishnets/WebSocketSession.hpp>
#include <ac/jalog/Log.hpp>

namespace {

class WsSession final : public fishnets::WebSocketSession {
public:
    void wsOpened() override {
        AC_JALOG(Info, "Session opened");
    }

    void wsClosed() override {
        AC_JALOG(Info, "Session closed");
    }

    void wsReceivedBinary(itlib::span<uint8_t> binary) override {
        AC_JALOG(Warning, "Received binary with size ", binary.size(), ". Ignoring");
    }

    void wsReceivedText(itlib::span<char> text) override {
        std::string_view str(text.data(), text.size());
        AC_JALOG(Info, "Received text ", str);
        if (m_sent.empty()) {
            m_sent = str;
            wsSend(m_sent);
        }
        else
        {
            AC_JALOG(Warning, "Previous send is not complete. Ignoring\n");
        }
    }

    void wsCompletedSend() override {
        m_sent.clear();
    }

    std::string m_sent;
};

} // namespace

fishnets::WebSocketSessionPtr makeWsSession(const fishnets::WebSocketEndpointInfo&) {
    return std::make_shared<WsSession>();
}
