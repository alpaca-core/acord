// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <acord/Frame.hpp>
#include <doctest/doctest.h>

TEST_CASE("empty frame") {
    acord::Frame f;
    CHECK(f.empty());
    CHECK(f.size() == 0);
    CHECK(!f.text());
    CHECK(f.binary());
}

TEST_CASE("text frame") {
    acord::Frame f;
    f.textBuffer = "hello";
    CHECK(!f.empty());
    CHECK(f.size() == 5);
    CHECK(f.text());
    CHECK(!f.binary());
}

TEST_CASE("binary frame") {
    acord::Frame f;
    f.binaryBuffer = {1, 2, 3};
    CHECK(!f.empty());
    CHECK(f.size() == 3);
    CHECK(!f.text());
    CHECK(f.binary());
}
