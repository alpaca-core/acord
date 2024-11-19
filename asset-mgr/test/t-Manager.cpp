// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <ac/asset/SourceLocalDir.hpp>
#include <ac/asset/Manager.hpp>
#include <test-assets/assets.h>
#include <doctest/doctest.h>
#include <astl/tsumap.hpp>
#include <latch>
#include <cstring>

const auto AnotherTestFile_DummySize = strlen(TA_ANOTHER_BINARY_FILE) + 1000; //Size reported by the Dummy asset source
const std::string Bin_Path = TEST_ASSETS_BINARY_PATH;

class DummyAssetSource : public ac::asset::Source{
public:
    virtual std::string_view id() const noexcept override {
        return "dummy";
    }

    virtual std::optional<BasicAssetInfo> checkAssetSync(std::string_view id) noexcept override {
        if (id.starts_with("test") || id.starts_with("no")) {
            return std::nullopt;
        }
        if (id.starts_with("local")) {
            return BasicAssetInfo{1'000'000 + id.size(), "loc/" + std::string(id)};
        }
        if (id.starts_with("sizeless")) {
            return BasicAssetInfo{};
        }
        return BasicAssetInfo{1'000 + id.size()};
    }
    virtual BasicAssetInfo fetchAssetSync(std::string_view id, ProgressCb progress) override {
        auto basicInfo = checkAssetSync(id);
        if (!progress(7)) { // dummy progress so we can check it
            return std::move(*basicInfo);
        }
        if (!basicInfo) {
            throw std::runtime_error("dummy not found");
        }
        if (id.starts_with("error")) {
            throw std::runtime_error("dummy error");
        }
        if (!basicInfo->path) {
            basicInfo->path = "dl/" + std::string(id);
        }
        if (!basicInfo->size) {
            basicInfo->size = 10'000 + id.length();
        }
        return std::move(*basicInfo);
    }
};

TEST_CASE("dummy-dir") {
    ac::asset::Manager mgr;

    mgr.addSource(ac::asset::SourceLocalDir_Create(Bin_Path), 10);
    mgr.addSource(std::make_unique<DummyAssetSource>());

    ac::asset::Info info;
    auto q = [&](std::string_view qid) {
        std::latch latch(1);
        mgr.queryAsset(std::string(qid), [&](std::string_view id, const ac::asset::Info& data) {
            CHECK(id == qid);
            info = data;
            latch.count_down();
        });
        latch.wait();
    };

    q("no-such-asset");
    CHECK_FALSE(info.source);
    CHECK_FALSE(info.size);
    CHECK_FALSE(info.path);
    CHECK(info.error == "Asset not found");

    q(TA_BINARY_FILE);
    REQUIRE(info.source);
    CHECK(info.source->id() == "local-dir: " + Bin_Path);
    CHECK(info.size == TA_BINARY_FILE_SIZE);
    CHECK(info.path == Bin_Path + "/" + TA_BINARY_FILE);
    CHECK_FALSE(info.error);

    q(TA_ANOTHER_BINARY_FILE);
    REQUIRE(info.source);
    CHECK(info.source->id() == "dummy"); // smaller prio
    CHECK(info.size == AnotherTestFile_DummySize);
    CHECK_FALSE(info.path);
    CHECK_FALSE(info.error);

    q("local-asset");
    REQUIRE(info.source);
    CHECK(info.source->id() == "dummy");
    CHECK(info.size == 1'000'000 + 11);
    CHECK(info.path == "loc/local-asset");
    CHECK_FALSE(info.error);

    q("sizeless-asset");
    REQUIRE(info.source);
    CHECK(info.source->id() == "dummy");
    CHECK_FALSE(info.size);
    CHECK_FALSE(info.path);
    CHECK_FALSE(info.error);

    auto g = [&](std::string_view qid, bool abort = false) {
        std::latch latch(1);
        mgr.getAsset(std::string(qid),
            [&](std::string_view id, const ac::asset::Info& data) {
                CHECK(id == qid);
                info = data;
                latch.count_down();
            },
            [&](std::string_view id, float f) {
                CHECK(id == qid);
                CHECK(f == 7);
                return !abort;
            }
        );
        latch.wait();
    };

    g("no-such-asset");
    CHECK_FALSE(info.source);
    CHECK_FALSE(info.size);
    CHECK_FALSE(info.path);
    CHECK(info.error == "Can't get asset. No source");

    g("yes-such-asset", true);
    CHECK(info.source);
    CHECK(info.size == 1000 + 14);
    CHECK_FALSE(info.path);
    CHECK_FALSE(info.error);

    g(TA_BINARY_FILE);
    REQUIRE(info.source);
    CHECK(info.source->id() == "local-dir: " + Bin_Path);
    CHECK(info.size == TA_BINARY_FILE_SIZE);
    CHECK(info.path == Bin_Path + "/" + TA_BINARY_FILE);
    CHECK_FALSE(info.error);

    g(TA_ANOTHER_BINARY_FILE);
    REQUIRE(info.source);
    CHECK(info.source->id() == "dummy"); // smaller prio
    CHECK(info.size == AnotherTestFile_DummySize);
    CHECK(info.path == std::string("dl/") + TA_ANOTHER_BINARY_FILE);
    CHECK_FALSE(info.error);

    g("local-asset");
    REQUIRE(info.source);
    CHECK(info.source->id() == "dummy");
    CHECK(info.size == 1'000'000 + 11);
    CHECK(info.path == "loc/local-asset");
    CHECK_FALSE(info.error);

    g("sizeless-asset-x");
    REQUIRE(info.source);
    CHECK(info.source->id() == "dummy");
    CHECK(info.size == 10'000 + 16);
    CHECK(info.path == "dl/sizeless-asset-x");
    CHECK_FALSE(info.error);

    g("error-asset");
    REQUIRE(info.source);
    CHECK(info.source->id() == "dummy");
    CHECK(info.size == 1000 + 11);
    CHECK_FALSE(info.path);
    CHECK(info.error == "dummy error");
}

TEST_CASE("dir-dummy") {
    ac::asset::Manager mgr;

    mgr.addSource(ac::asset::SourceLocalDir_Create(Bin_Path), -10);
    mgr.addSource(std::make_unique<DummyAssetSource>());

    ac::asset::Info info;
    std::latch latch(1);
    mgr.queryAsset(std::string(TA_ANOTHER_BINARY_FILE), [&](std::string_view id, const ac::asset::Info& data) {
        CHECK(id == TA_ANOTHER_BINARY_FILE);
        info = data;
        latch.count_down();
    });
    latch.wait();

    REQUIRE(info.source);
    CHECK(info.source->id() == "local-dir: " + Bin_Path);
    CHECK(info.size == TA_ANOTHER_BINARY_FILE_SIZE);
    CHECK(info.path == Bin_Path + "/" + TA_ANOTHER_BINARY_FILE);
    CHECK_FALSE(info.error);
}

TEST_CASE("external execution") {
    ac::asset::Manager mgr(ac::asset::Manager::No_LaunchThread);
    mgr.addSource(std::make_unique<DummyAssetSource>());

    astl::tsumap<ac::asset::Info> infos;

    auto qcb = [&infos](std::string_view id, const ac::asset::Info& data) {
        infos[std::string(id)] = data;
    };

    mgr.queryAsset("nope", qcb);
    mgr.queryAsset("local", qcb);
    mgr.queryAsset("sizeless", qcb);
    mgr.pushStop();

    mgr.run();

    CHECK(infos.size() == 3);

    {
        auto& info = infos["nope"];
        CHECK_FALSE(info.source);
        CHECK_FALSE(info.size);
        CHECK_FALSE(info.path);
        CHECK(info.error == "Asset not found");
    }

    {
        auto& info = infos["local"];
        REQUIRE(info.source);
        CHECK(info.source->id() == "dummy");
        CHECK(info.size == 1'000'000 + 5);
        CHECK(info.path == "loc/local");
        CHECK_FALSE(info.error);
    }

    {
        auto& info = infos["sizeless"];
        REQUIRE(info.source);
        CHECK(info.source->id() == "dummy");
        CHECK_FALSE(info.size);
        CHECK_FALSE(info.path);
        CHECK_FALSE(info.error);
    }
}
