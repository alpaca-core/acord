// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <ac/asset/SourceLocalDir.hpp>
#include <test-assets/assets.h>
#include <doctest/doctest.h>

TEST_CASE("SourceLocalDir") {
    const std::string Bin_Path = TEST_ASSETS_BINARY_PATH;

    auto src = ac::asset::SourceLocalDir_Create(Bin_Path);
    REQUIRE(src);

    CHECK(src->id() == "local-dir: " + Bin_Path);

    {
        auto info = src->checkAssetSync("no-such-file");
        CHECK_FALSE(info);
    }

    {
        CHECK_THROWS_WITH_AS(src->fetchAssetSync("no-such-file", {}), "Asset not found", std::runtime_error);
    }

    const std::string asset = TA_BINARY_FILE;

    auto checkSuccess = [&](ac::asset::Source::BasicAssetInfo& info) {
        CHECK(info.path.value_or(std::string{}) == Bin_Path + "/" + asset);
        CHECK(info.size.value_or(0) == TA_BINARY_FILE_SIZE); // The size of our new binary file
    };

    {
        auto info = src->checkAssetSync(asset);
        REQUIRE(info);
        checkSuccess(*info);
    }

    {
        auto info = src->fetchAssetSync(asset, {});
        checkSuccess(info);
    }
}
