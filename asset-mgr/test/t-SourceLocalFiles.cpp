// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <ac/asset/SourceLocalFiles.hpp>
#include <test-assets/assets.h>
#include <ac-test-util/setenv.hpp>
#include <doctest/doctest.h>

TEST_CASE("SourceLocalFiles") {
    setenv("AC_BIN_DIR", TEST_ASSETS_BINARY_PATH, 1);

    ac::asset::SourceLocalFiles src("test", R"({
        "foo": ")" TEST_ASSETS_BINARY_PATH "/" TA_BINARY_FILE R"(",
        "bar": "$AC_BIN_DIR/)" TA_ANOTHER_BINARY_FILE R"(",
        "baz": "$AC_BIN_DIR/no-such-file"
    })");

    CHECK(src.id() == "test");

    {
        auto info = src.checkAssetSync("zzz");
        REQUIRE_FALSE(info);
    }

    CHECK_THROWS_WITH_AS(src.fetchAssetSync("zzz", {}), "Asset not found: zzz", std::runtime_error);

    {
        auto info = src.checkAssetSync("baz");
        REQUIRE(info);
        CHECK_FALSE(info->path);
        CHECK_FALSE(info->size);
    }

    CHECK_THROWS_WITH_AS(src.fetchAssetSync("baz", {}),
        "File not found: $AC_BIN_DIR/no-such-file", std::runtime_error);

    {
        auto oinfo = src.checkAssetSync("foo");
        REQUIRE(oinfo);
        CHECK(oinfo->path == TEST_ASSETS_BINARY_PATH "/" TA_BINARY_FILE);
        CHECK(oinfo->size == TA_BINARY_FILE_SIZE);

        auto info = src.fetchAssetSync("foo", {});
        CHECK(info == oinfo);
    }

    {
        auto oinfo = src.checkAssetSync("bar");
        REQUIRE(oinfo);
        CHECK(oinfo->path == TEST_ASSETS_BINARY_PATH "/" TA_ANOTHER_BINARY_FILE);
        CHECK(oinfo->size == TA_ANOTHER_BINARY_FILE_SIZE);

        auto info = src.fetchAssetSync("bar", {});
        CHECK(info == oinfo);
    }
}
