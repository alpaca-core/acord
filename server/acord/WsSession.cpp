// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "WsSession.hpp"
#include "LocalSession.hpp"
#include "AppCtx.hpp"

#include <fishnets/WebSocketSession.hpp>

#include <ac/frameio/local/LocalIoCtx.hpp>
#include <ac/frameio/local/LocalBufferedChannel.hpp>
#include <ac/frameio/local/LocalChannelUtil.hpp>

#include <ac/jalog/Log.hpp>

#include <astl/shared_from.hpp>
#include <deque>

namespace acord::server {
namespace {

class WsSession final : public fishnets::WebSocketSession, public astl::enable_shared_from {
    const AppCtx& m_appCtx;

    ac::frameio::StreamEndpoint m_dispatch;
public:
    WsSession(const AppCtx& ctx) : m_appCtx(ctx)
    {}

    void wsOpened() override {
        AC_JALOG(Info, "Session opened");
        auto ca = ac::frameio::LocalBufferedChannel_create(10);
        auto cb = ac::frameio::LocalBufferedChannel_create(10);

        auto [local, remote] = ac::frameio::LocalChannel_getEndpoints(ca, cb);
        m_dispatch = std::move(local);
        m_appCtx.io.connect(m_appCtx.sessionFactory.createHandler(), std::move(remote));

        tryReadFromLocalSession();
    }

    void wsClosed() override {
        AC_JALOG(Info, "Session closed");
        m_dispatch.readStream->close();
        m_dispatch.writeStream->close();
    }

    void wsReceivedBinary(itlib::span<uint8_t> binary) override {
        AC_JALOG(Warning, "Received binary with size ", binary.size(), ". Ignoring");
    }

    void tryWriteToLocalSession() {
        while (true) {
            if (m_received.empty()) {
                return;
            }
            auto res = m_dispatch.writeStream->write(m_received.front(), [&] {
                return [this, pl = shared_from_this()] {
                    postWSIOTask([this]() {
                        tryWriteToLocalSession();
                    });
                };
            });
            if (res.blocked()) break;
            if (res.closed()) {
                wsClose();
                return;
            }
            assert(res.success());
            m_received.pop_front();
        }
    }

    void wsReceivedText(itlib::span<char> text) override {
        auto& frame = m_received.emplace_back();

        auto json = ac::Dict::parse(text.begin(), text.end());
        frame.op = json["op"].get<std::string>();
        frame.data = std::move(json["data"]);

        if (m_received.size() == 1) {
            tryWriteToLocalSession();
        }
    }

    void tryReadFromLocalSession() {
        bool alsoSend = m_sending.empty();

        while (m_sending.size() < 2) {
            auto& buf = m_sending.emplace_back();
            ac::Frame frame;
            auto res = m_dispatch.readStream->read(frame, [this] {
                return [this, pl = shared_from_this()] {
                    postWSIOTask([this]() {
                        tryReadFromLocalSession();
                    });
                };
            });

            if (res.success()) {
                ac::Dict json;
                json["op"] = std::move(frame.op);
                json["data"] = std::move(frame.data);
                buf = json.dump();
                continue;
            }

            m_sending.pop_back();
            if (res.closed()) {
                wsClose();
            }
            assert(res.closed() || res.blocked());
            break;
        }

        if (alsoSend) {
            doSend();
        }
    }

    void doSend() {
        if (m_sending.empty()) return;
        auto& data = m_sending.front();
        wsSend(data);
    }

    void wsCompletedSend() override {
        m_sending.pop_front();
        if (m_sending.size() < 2) {
            tryReadFromLocalSession();
        }
        doSend();
    }

    std::deque<ac::Frame> m_received;
    std::deque<std::string> m_sending;
};

} // namespace

fishnets::WebSocketSessionPtr makeWsSession(const AppCtx& ctx) {
    return std::make_shared<WsSession>(ctx);
}
} // namespace acord::server
