// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "WsSession.hpp"
#include "LocalSession.hpp"
#include "AppCtx.hpp"
#include "Logging.hpp"

#include <acord/CommonWsSession.hpp>

#include <ac/frameio/local/LocalIoCtx.hpp>
#include <ac/frameio/local/LocalBufferedChannel.hpp>
#include <ac/frameio/local/LocalChannelUtil.hpp>

namespace acord::server {
namespace {

class WsSession final : public CommonWsSession {
    const AppCtx& m_appCtx;
public:
    WsSession(const AppCtx& ctx) : m_appCtx(ctx)
    {}

    void wsOpened() override {
        ACORD_SRV_LOG(Info, "Session opened");
        auto [local, remote] = LocalChannel_getEndpoints(
            ac::frameio::LocalBufferedChannel_create(10),
            ac::frameio::LocalBufferedChannel_create(10)
        );

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
