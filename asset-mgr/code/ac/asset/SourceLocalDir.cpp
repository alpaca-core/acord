// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "SourceLocalDir.hpp"
#include "FsUtil.hpp"
#include <astl/move.hpp>
#include <stdexcept>

namespace ac::asset {

namespace {
class SourceLocalDir final : public Source {
    std::string m_path;
    std::string m_id;
public:
    SourceLocalDir(std::string path)
        : m_path(astl::move(path))
        , m_id("local-dir: " + m_path)
    {}

    virtual std::string_view id() const noexcept override {
        return m_id;
    }

    virtual std::optional<BasicAssetInfo> checkAssetSync(std::string_view id) noexcept override {
        auto path = m_path + "/" + std::string(id);
        auto st = fs::basicStat(path);
        if (st.file()) {
            return BasicAssetInfo{st.size, astl::move(path)};
        }
        return std::nullopt;
    }

    virtual BasicAssetInfo fetchAssetSync(std::string_view id, ProgressCb) override {
        auto ret = checkAssetSync(id);
        if (ret) {
            return astl::move(*ret);
        }
        else {
            throw std::runtime_error("Asset not found");
        }
    }
};
}

std::unique_ptr<Source> SourceLocalDir_Create(std::string path) {
    return std::make_unique<SourceLocalDir>(astl::move(path));
}
}
