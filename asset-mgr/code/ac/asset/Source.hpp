// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include <string_view>
#include <functional>
#include <optional>
#include <string>

namespace ac::asset {

class AC_ASSET_EXPORT Source {
public:
    virtual ~Source();

    virtual std::string_view id() const noexcept = 0;

    // synchronous interface for now (may be redesigned later)

    struct BasicAssetInfo {
        std::optional<size_t> size;
        std::optional<std::string> path;

        friend bool operator==(const BasicAssetInfo&, const BasicAssetInfo&) = default;
    };

    // return nullopt if the source does not have the asset
    virtual std::optional<BasicAssetInfo> checkAssetSync(std::string_view id) noexcept = 0;

    // return false in progressCb to abort the operation
    using ProgressCb = std::function<bool(float)>;
    virtual BasicAssetInfo fetchAssetSync(std::string_view id, ProgressCb progressCb) = 0;
};

} // namespace ac::asset
