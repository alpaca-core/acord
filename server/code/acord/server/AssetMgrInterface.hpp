// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <ac/schema/Field.hpp>
#include <vector>
#include <string>
#include <tuple>

namespace ac::schema {

inline namespace amgr {

struct State {
    static constexpr auto id = "asset-mgr";
    static constexpr auto desc = "Asset manager state";

    struct OpMakeAssetsAvailable {
        static constexpr auto id = "make-assets-available";
        static constexpr auto desc = "Make assets available";

        using Params = std::vector<std::string>;

        using Return = std::vector<std::string>;
    };

    using Ops = std::tuple<OpMakeAssetsAvailable>;
    using Ins = std::tuple<>;
    using Outs = std::tuple<>;
};

struct Interface {
    static inline constexpr std::string_view id = "$asset-mgr";
    static inline constexpr std::string_view desc = "Asset manager interface";

    using States = std::tuple<State>;
};

} // namespace amgr

} // namespace ac::schema
