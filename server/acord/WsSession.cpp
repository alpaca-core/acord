// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "WsSession.hpp"
#include "LocalSession.hpp"
#include "AppCtx.hpp"

#include <acord/FrameCvt.hpp>

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

    FrameCvt m_cvt = FrameCvt::json();
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

    template <typename T>
    void received(itlib::span<T> span) {
        auto& frame = m_received.emplace_back();

        try {
            if constexpr (std::is_same_v<T, char>) {
                m_cvt.jsonBufToAc(frame, {span.data(), span.size()});
            }
            else {
                m_cvt.cborBufToAc(frame, {span.data(), span.size()});
            }
        }
        catch (std::exception& ex) {
            AC_JALOG(Error, "Error reading frame: ", ex.what());
            wsClose();
            return;
        }

        if (m_received.size() == 1) {
            tryWriteToLocalSession();
        }
    }

    void wsReceivedText(itlib::span<char> text) override {
        received(text);
    }

    void wsReceivedBinary(itlib::span<uint8_t> binary) override {
        received(binary);
    }

    void tryReadFromLocalSession() {
        if (m_sending) return;
        auto& buf = m_sending.emplace();

        ac::Frame frame;
        auto res = m_dispatch.readStream->read(frame, [this] {
            return [this, pl = shared_from_this()] {
                postWSIOTask([this]() {
                    tryReadFromLocalSession();
                });
            };
        });

        if (res.success()) {
            m_cvt.fromAcFrame(buf, frame);
            doSend();
            return;
        }

        m_sending.reset();

        if (res.closed()) {
            wsClose();
        }

        assert(res.closed() || res.blocked());
    }

    void doSend() {
        assert(!!m_sending);
        if (m_sending->text()) {
            wsSend(m_sending->textBuf);
        }
        else {
            wsSend(m_sending->binaryBuf);
        }
    }

    void wsCompletedSend() override {
        m_sending.reset();
        tryReadFromLocalSession();
    }

    std::deque<ac::Frame> m_received;
    std::optional<acord::Frame> m_sending;
};

} // namespace

fishnets::WebSocketSessionPtr makeWsSession(const AppCtx& ctx) {
    return std::make_shared<WsSession>(ctx);
}
} // namespace acord::server
