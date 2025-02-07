// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "LocalSession.hpp"

#include "AssetMgr.hpp"

#include <acord/schema/Acord.hpp>
#include <ac/schema/FrameHelpers.hpp>
#include <ac/schema/OpDispatchHelpers.hpp>
#include <ac/FrameUtil.hpp>

#include <ac/frameio/SessionCoro.hpp>
#include <ac/frameio/StreamEndpoint.hpp>
#include <ac/local/Lib.hpp>

#include <ac/jalog/Log.hpp>

#include <astl/throw_stdex.hpp>

namespace acord::server {

using throw_ex = ac::throw_ex;

namespace {
namespace coro = ac::frameio::coro;
using Schema = ac::schema::acord::State;

ac::frameio::SessionCoro<void> Acord_makeAssetsAvailable(coro::Io& io, std::vector<std::string> urls, AssetMgr& assetMgr) {
    auto aio = io.attach(assetMgr.makeAssetsAvailable(std::move(urls)));
    auto result = co_await aio.pollFrame();
    result.frame.op = Schema::OpMakeAssetsAvailable::id;
    co_await io.pushFrame(std::move(result.frame));
}

ac::frameio::SessionCoro<void> Acord_run(AssetMgr& assetMgr) {
    try {
        auto io = co_await coro::Io{};

        co_await io.pushFrame(ac::Frame_stateChange(Schema::id));

        while (true) {
            auto f = co_await io.pollFrame();
            if (f.frame.op == Schema::OpMakeAssetsAvailable::id) {
                co_await Acord_makeAssetsAvailable(
                    io,
                    ac::schema::Struct_fromDict<Schema::OpMakeAssetsAvailable::Params>(std::move(f.frame.data)),
                    assetMgr
                );
            }
            else {
                co_await io.pushFrame({"error", "Unknown op: " + f.frame.op});
            }
        }
    }
    catch (ac::frameio::IoClosed&) {
        co_return;
    }
}
} // namespace

ac::frameio::SessionHandlerPtr LocalSessionFactory::createHandler() {
    return ac::frameio::CoroSessionHandler::create(Acord_run(assetMgr));
    // return ac::local::Lib::createSessionHandler("foo");
}
} // namespace acord::server
