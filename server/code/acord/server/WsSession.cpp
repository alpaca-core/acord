// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "WsSession.hpp"
#include "LocalSession.hpp"
#include "AppCtx.hpp"
#include "Logging.hpp"

#include <acord/CommonWsSession.hpp>

#include <ac/frameio/local/BufferedChannel.hpp>
#include <ac/frameio/local/BufferedChannelStream.hpp>

namespace acord::server {
namespace {

class WsSession final : public CommonWsSession {
    const AppCtx& m_appCtx;
public:
    WsSession(const AppCtx& ctx) : m_appCtx(ctx)
    {}

    void wsOpened(std::string_view target) override {
        CommonWsSession::wsOpened(target);
        ACORD_SRV_LOG(Info, "Session opened");
        auto [local, remote] = ac::frameio::BufferedChannel_getEndpoints(
            ac::frameio::BufferedChannel_create(10),
            ac::frameio::BufferedChannel_create(10)
        );

        m_dispatch = std::move(local);
        LocalSession_connect(m_appCtx, std::move(remote));

        tryReadFromDispatch();
    }
};

} // namespace

fishnets::WsSessionHandlerPtr makeWsSession(const AppCtx& ctx) {
    return std::make_shared<WsSession>(ctx);
}
} // namespace acord::server
