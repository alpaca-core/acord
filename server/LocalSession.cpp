// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "LocalSession.hpp"
#include <ac/frameio/SessionCoro.hpp>
#include <ac/local/Lib.hpp>

#include <ac/jalog/Log.hpp>

namespace {
ac::frameio::SessionCoro<void> localSession() {
    auto io = co_await ac::frameio::coro::Io{};
    while (true) {
        auto res = co_await io.pollFrame();
        auto& frame = res.frame;

        AC_JALOG(Info, "Received frame ", frame.op);

        if (frame.op == "end") {
            co_return;
        }

        co_await io.pushFrame(frame);
    }
}
} // namespace

ac::frameio::SessionHandlerPtr LocalSessionFactory::createHandler() {
    // return ac::frameio::CoroSessionHandler::create(localSession());
    return ac::local::Lib::createSessionHandler("foo");
}
