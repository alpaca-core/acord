// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <acord/FrameCvt.hpp>
#include <doctest/doctest.h>

TEST_CASE("json") {
    auto cvt = acord::FrameCvt::json();

    acord::Frame af;

    {
        ac::Frame f = {
            "some op",
            ac::Dict{
                {"key1", "value1"},
                {"key2", 42}
            }
        };
        cvt.fromAcFrame(af, f);
    }

    {
        ac::Frame f;
        cvt.toAcFrame(f, af);
        CHECK(f.op == "some op");
        CHECK(f.data.size() == 2);
        CHECK(f.data["key1"].get<std::string_view>() == "value1");
        CHECK(f.data["key2"].get<int>() == 42);
    }
}

TEST_CASE("cbor") {
    auto cvt = acord::FrameCvt::cbor();

    ac::Blob init{1, 2, 3};
    auto idata = init.data();

    acord::Frame af;
    {
        ac::Frame f = {
            "some op",
            ac::Dict{
                {"key1", "value1"},
                {"key2", 42},
                {"blob", ac::Dict::binary(std::move(init))}
            }
        };
        cvt.fromAcFrame(af, f);
    }

    {
        ac::Frame f;
        cvt.toAcFrame(f, af);
        CHECK(f.op == "some op");
        CHECK(f.data.size() == 3);
        CHECK(f.data["key1"].get<std::string_view>() == "value1");
        CHECK(f.data["key2"].get<int>() == 42);
        auto& jblob = f.data["blob"];
        CHECK(jblob.is_binary());
        ac::Blob& blob = jblob.get_binary();
        CHECK(blob == ac::Blob{1, 2, 3});
        CHECK(blob.data() == idata); // all moves
    }
}