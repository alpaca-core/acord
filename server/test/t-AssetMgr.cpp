// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <acord/server/AssetMgrService.hpp>
#include <acord/server/AssetMgrInterface.hpp>

#include <acord/server/FsUtil.hpp>
#include <ac/frameio/StreamEndpoint.hpp>
#include <ac/schema/BlockingIoHelper.hpp>
#include <ac/local/DefaultBackend.hpp>
#include <doctest/doctest.h>

#include <ac/jalog/Fixture.inl>

using namespace acord::server;

namespace schema = ac::schema::amgr;

TEST_CASE("dl") {
    std::vector<std::string> urls = {
        "https://raw.githubusercontent.com/alpaca-core/test-data-foo/900479ceedddc6e3867c467bbedb7a5310414041/foo-large.txt",
    };

    ac::local::DefaultBackend backend;
    backend.registerService(AssetMgr_getServiceFactory());

    ac::schema::BlockingIoHelper io(backend.connect(schema::Interface::id, {}));

    io.expectState<schema::State>();

    auto paths = io.call<schema::State::OpMakeAssetsAvailable>(urls);

    CHECK(paths.size() == 1);

    auto& fname = paths.front();
    CHECK(fname.ends_with("foo-large.txt"));
    CHECK(fs::basicStat(fname).file());

    fs::rm_r(fname);
}
