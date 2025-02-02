// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "WsSession.hpp"
#include "LocalSession.hpp"
#include "AppCtx.hpp"

#include <acord/CommonWsSession.hpp>

#include <fishnets/WebSocketSession.hpp>

#include <ac/frameio/local/LocalIoCtx.hpp>
#include <ac/frameio/local/LocalBufferedChannel.hpp>
#include <ac/frameio/local/LocalChannelUtil.hpp>

#include <ac/jalog/Log.hpp>

#include <astl/shared_from.hpp>
#include <deque>

namespace acord::server {
namespace {

class WsSession final : public CommonWsSession {
    const AppCtx& m_appCtx;
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

        tryReadFromDispatch();
    }
};

} // namespace

fishnets::WebSocketSessionPtr makeWsSession(const AppCtx& ctx) {
    return std::make_shared<WsSession>(ctx);
}
} // namespace acord::server
