// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <ac/asset/SourceHttp.hpp>
#include <ac/asset/FsUtil.hpp>
#include <test-assets/assets.h>
#include <itlib/sentry.hpp>
#include "ac-repo-root.h"
#include <doctest/doctest.h>
#include <fstream>

using namespace ac::asset;

TEST_CASE("AssetSourceHttp") {
    const std::string dir = AC_REPO_TMP_DIR "/test-http";
    fs::mkdir_p(dir);
    itlib::sentry cleanup([&] { fs::rm_r(dir, true); });

    std::vector<uint8_t> data(80);
    for (size_t i = 0; i < data.size(); ++i) {
        data[i] = i & 0xFF;
    }
    {
        std::ofstream f(dir + "/existing", std::ios::binary);
        f.write(reinterpret_cast<const char*>(data.data()), data.size());
    }

    SourceHttp src("test", dir);
    src.addAsset("existing", {
        .url = "should never be used",
        .xxhash = 42
    });
    src.addAsset("bytes-150", {
        .url = "https://httpbin.org/bytes/150?seed=15",
        //.xxhash = 0XBABABAull
    });

    CHECK_FALSE(src.checkAssetSync("foo"));

    {
        auto info = src.checkAssetSync("existing");
        REQUIRE(info);
        CHECK(info->path == dir + "/existing");
        CHECK(info->size == data.size());
    }

    {
        auto info = src.checkAssetSync("bytes-150");
        REQUIRE(info);
        CHECK_FALSE(info->path);
        CHECK_FALSE(info->size);
    }

    CHECK_THROWS_WITH_AS(src.fetchAssetSync("foo", {}), "Asset not found: foo", std::runtime_error);

    std::optional<float> progress = 0.f;
    auto pcb = [&](float p) {
        progress = p;
        return true;
    };

    {
        auto info = src.fetchAssetSync("existing", pcb);
        CHECK(info.path == dir + "/existing");
        CHECK(info.size == data.size());
        CHECK(progress == 0);
    }


    {
        auto info = src.fetchAssetSync("bytes-150", [](float) { return false; });
        // aborted, but still the size should be available at this point
        CHECK_FALSE(info.path);
        CHECK(*info.size == 150);
    }

    progress.reset();

    {
        auto info = src.fetchAssetSync("bytes-150", pcb);
        CHECK(info.path == dir + "/bytes-150");
        CHECK(*info.size == 150);
        CHECK(progress == 1);
    }

    // bytes-150 should be available from now on

    {
        auto info = src.checkAssetSync("bytes-150");
        REQUIRE(info);
        CHECK(info->path == dir + "/bytes-150");
        CHECK(*info->size == 150);
    }

    {
        progress = 8.f;
        auto info = src.fetchAssetSync("bytes-150", pcb);
        CHECK(info.path == dir + "/bytes-150");
        CHECK(*info.size == 150);
        CHECK(progress == 8); // the progress callback should not have been called
    }
}
