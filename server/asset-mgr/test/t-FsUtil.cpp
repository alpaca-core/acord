// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <acord/asset/FsUtil.hpp>
#include <test-assets.h>
#include <astl/tu-setenv.hpp>
#include <doctest/doctest.h>

TEST_CASE("expandPath") {
    // can't test home without reinforcing bugs
    {
        auto e = acord::asset::fs::expandPath("~/foo");

        CHECK(e.ends_with("/foo"));

#if defined(_WIN32)
        REQUIRE(e.length() > 5);
        CHECK(e[1] == ':'); // must be absolute
#else
        CHECK(e.length() >= 4);
        CHECK(e[0] == '/'); // must be absolute
#endif
    }

    auto test = [](const std::string& path, std::string_view expected) {
        auto expanded = acord::asset::fs::expandPath(path);
        CHECK(expanded == expected);
        auto inplace = path;
        acord::asset::fs::expandPathInPlace(inplace);
        CHECK(inplace == expected);
    };

    setenv("AC_TEST_VAR", "foo", 1);
    test("$AC_TEST_VAR", "foo");
    test("$AC_TEST_VAR/bar", "foo/bar");

    CHECK_THROWS_WITH_AS(acord::asset::fs::expandPath("$nope42/xxx"), "Environment variable nope42 not set", std::runtime_error);
}

const std::string binDir = TEST_ASSETS_BINARY_PATH;
const std::string bin1 = TEST_ASSETS_BINARY_PATH "/" TA_BINARY_FILE;
const std::string bin2 = TEST_ASSETS_BINARY_PATH "/" TA_ANOTHER_BINARY_FILE;
const std::string nope = TEST_ASSETS_BINARY_PATH "/no-such-file";

TEST_CASE("stat") {
    auto s = acord::asset::fs::basicStat(binDir);
    CHECK(s.exists());
    CHECK(s.dir());
    CHECK(!s.file());
    CHECK(s.size == 0);

    s = acord::asset::fs::basicStat(bin1);
    CHECK(s.exists());
    CHECK(s.file());
    CHECK(!s.dir());
    CHECK(s.size == TA_BINARY_FILE_SIZE);

    s = acord::asset::fs::basicStat(bin2);
    CHECK(s.exists());
    CHECK(s.file());
    CHECK(!s.dir());
    CHECK(s.size == TA_ANOTHER_BINARY_FILE_SIZE);

    s = acord::asset::fs::basicStat(nope);
    CHECK(!s.exists());
    CHECK(!s.file());
    CHECK(!s.dir());
    CHECK(s.size == 0);
}

TEST_CASE("cmd") {
    using namespace acord::asset::fs;

    const std::string dir = getTempDir() + "/fs-test";

    mkdir_p(dir);
    auto st = basicStat(dir);
    CHECK(st.dir());

    std::string deepDir = dir + "/dir/which/is/deep";
    mkdir_p(deepDir);
    st = basicStat(deepDir);
    CHECK(st.dir());

    std::string touched = dir + "/touched";
    touch(touched);
    st = basicStat(touched);
    CHECK(st.file());

    rm_r(touched);
    st = basicStat(touched);
    CHECK(!st.exists());

    std::string deepTouched = dir + "/deep/touched/file";
    {
        auto ex = "Failed to touch file: " + deepTouched;
        CHECK_THROWS_WITH_AS(touch(deepTouched, false), ex.c_str(), std::runtime_error);
    }
    touch(deepTouched);
    st = basicStat(deepTouched);
    CHECK(st.file());

    CHECK_THROWS(rm_r(dir));

    rm_r(dir, true);
    st = basicStat(dir);
    CHECK(!st.exists());
}
