// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "WsSession.hpp"

#include <acord/CommonWsSession.hpp>

#include <ac/jalog/Log.hpp>

namespace acord::client {
namespace {

class WsSession final : public CommonWsSession {
public:
    WsSession()
    {}

    void wsOpened() override {
    }
};

} // namespace

fishnets::WebSocketSessionPtr makeWsSession() {
    return {};
}
} // namespace acord::client
