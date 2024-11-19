// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include "Source.hpp"
#include <astl/tsumap.hpp>

namespace ac::asset {
class AC_ASSET_EXPORT SourceLocalFiles final : public Source {
public:
    SourceLocalFiles(std::string_view id, const astl::tsumap<std::string>& manifest);
    SourceLocalFiles(std::string_view id, std::string_view jsonManifest);

    virtual std::string_view id() const noexcept override { return m_id; }

    virtual std::optional<BasicAssetInfo> checkAssetSync(std::string_view id) noexcept override;
    virtual BasicAssetInfo fetchAssetSync(std::string_view id, ProgressCb) override;
private:
    std::string m_id;

    struct AssetManifestEntry {
        BasicAssetInfo info;
        std::string path;
    };
    astl::tsumap<AssetManifestEntry> m_assetManifest; // id -> existing file
};
} // namespace ac::asset
