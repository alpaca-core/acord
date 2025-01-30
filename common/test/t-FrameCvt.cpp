// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <acord/FrameCvt.hpp>
#include <doctest/doctest.h>

TEST_CASE("json") {
    acord::Frame af;

    {
        ac::Frame f = {
            "some op",
            ac::Dict{
                {"key1", "value1"},
                {"key2", 42}
            }
        };
        acord::FrameCvt::jsonFrameFromAc(af, f);
    }

    CHECK(af.text());

    {
        ac::Frame f;
        acord::FrameCvt::jsonBufToAc(f, af.textBuffer);
        CHECK(f.op == "some op");
        CHECK(f.data.size() == 2);
        CHECK(f.data["key1"].get<std::string_view>() == "value1");
        CHECK(f.data["key2"].get<int>() == 42);
    }
}

TEST_CASE("binary") {
    acord::Frame af;
    {
        ac::Frame f = {
            "some op",
            ac::Dict{
                {"key1", "value1"},
                {"key2", 42},
                {"blob", ac::Dict::binary({1, 2, 3})}
            }
        };
        acord::FrameCvt::cborFrameFromAc(af, f);
    }

    CHECK(af.binary());

    {
        ac::Frame f;
        acord::FrameCvt::cborBufToAc(f, af.binaryBuffer);
        CHECK(f.op == "some op");
        CHECK(f.data.size() == 3);
        CHECK(f.data["key1"].get<std::string_view>() == "value1");
        CHECK(f.data["key2"].get<int>() == 42);
        auto& jblob = f.data["blob"];
        CHECK(jblob.is_binary());
        ac::Blob& blob = jblob.get_binary();
        CHECK(blob == ac::Blob{1, 2, 3});
    }
}

TEST_CASE("generic") {
    auto cvt = acord::FrameCvt::json();
    CHECK(cvt.fromAcFrame == acord::FrameCvt::jsonFrameFromAc);

    cvt = acord::FrameCvt::cbor();
    CHECK(cvt.fromAcFrame == acord::FrameCvt::cborFrameFromAc);
}
