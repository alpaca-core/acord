// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "LocalSession.hpp"

#include "AssetMgr.hpp"
#include "AppCtx.hpp"

#include <acord/schema/Acord.hpp>
#include <ac/schema/FrameHelpers.hpp>
#include <ac/schema/OpDispatchHelpers.hpp>
#include <ac/FrameUtil.hpp>

#include <ac/frameio/IoEndpoint.hpp>
#include <ac/local/Lib.hpp>
#include <ac/local/IoCtx.hpp>

#include <ac/xec/coro.hpp>

#include <ac/jalog/Log.hpp>

#include <astl/throw_stdex.hpp>

namespace acord::server {

using throw_ex = ac::throw_ex;

namespace {
using Schema = ac::schema::acord::State;

ac::xec::coro<void> Acord_makeAssetsAvailable(ac::frameio::IoEndpoint& io, std::vector<std::string> urls, AssetMgr& assetMgr) {
    auto ex = io.get_executor();
    ac::frameio::IoEndpoint aio(assetMgr.makeAssetsAvailable(std::move(urls)), ex);
    auto result = co_await aio.poll();
    result.value.op = Schema::OpMakeAssetsAvailable::id;
    co_await io.push(std::move(result.value));
}

ac::xec::coro<void> Acord_run(const AppCtx& appCtx, ac::frameio::StreamEndpoint ep) {
    try {
        auto ex = co_await ac::xec::executor{};
        ac::frameio::IoEndpoint io(std::move(ep), ex);

        co_await io.push(ac::Frame_stateChange(Schema::id));

        while (true) {
            auto f = co_await io.poll();
            if (f.value.op == Schema::OpMakeAssetsAvailable::id) {
                co_await Acord_makeAssetsAvailable(
                    io,
                    ac::schema::Struct_fromDict<Schema::OpMakeAssetsAvailable::Params>(std::move(f.value.data)),
                    appCtx.assetMgr
                );
            }
            else if (f.value.op == Schema::OpLoadProvider::id) {
                std::string error;
                try {
                    auto name = ac::schema::Struct_fromDict<Schema::OpLoadProvider::Params>(std::move(f.value.data)).name;
                    assert(name.hasValue());
                    auto& provider = ac::local::Lib::getProvider(name.value());

                    co_await io.push({Schema::OpLoadProvider::id, {}});

                    auto transfer = io.detach();
                    appCtx.ioCtx.attach(provider, std::move(transfer));
                    co_return;
                }
                catch (std::exception& e) {
                    error = e.what();
                }
                co_await io.push({"error", std::move(error)});
            }
            else {
                co_await io.push({"error", "Unknown op: " + f.value.op});
            }
        }
    }
    catch (ac::io::stream_closed_error&) {
        // nothing to do
    }
}
} // namespace

void LocalSession_connect(const AppCtx& ctx, ac::frameio::StreamEndpoint ep) {
    co_spawn(ctx.localSessionXCtx, Acord_run(ctx, std::move(ep)));
}
} // namespace acord::server
