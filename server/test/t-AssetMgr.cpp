// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <acord/server/AssetMgr.hpp>
#include <acord/server/FsUtil.hpp>
#include <ac/frameio/StreamEndpoint.hpp>
#include <ac/frameio/BlockingIo.hpp>
#include <doctest/doctest.h>

#include <ac/jalog/Fixture.inl>

using namespace acord::server;

TEST_CASE("noop") {
    AssetMgr mgr;
}

TEST_CASE("dl") {
    ac::frameio::BlockingIoCtx ctx;
    AssetMgr mgr;

    std::vector<std::string> urls = {
        "https://raw.githubusercontent.com/alpaca-core/test-data-foo/900479ceedddc6e3867c467bbedb7a5310414041/foo-large.txt",
    };

    auto ep = mgr.makeAssetsAvailable(urls);

    ac::frameio::BlockingIo io(std::move(ep), ctx);

    auto res = io.poll();

    CHECK(res.success());
    auto& frame = res.value;
    CHECK(frame.op == "assets");
    auto paths = frame.data.get<std::vector<std::string>>();
    CHECK(paths.size() == 1);

    auto& fname = paths.front();
    CHECK(fname.ends_with("foo-large.txt"));
    CHECK(fs::basicStat(fname).file());

    fs::rm_r(fname);
}
