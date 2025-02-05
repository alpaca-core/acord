// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <ac/schema/Field.hpp>
#include <vector>
#include <string>
#include <tuple>

namespace ac::schema {

inline namespace acord {

struct State {
    static constexpr auto id = "acord";
    static constexpr auto desc = "Initial acord state";

    struct OpMakeAssetsAvailable {
        static constexpr auto id = "make_assets_available";
        static constexpr auto desc = "Make assets available";

        using Params = std::vector<std::string>;
        using Return = std::vector<std::string>;
    };

    struct StreamProgress {
        static constexpr auto id = "progress";
        static constexpr auto desc = "Progress stream";

        struct Type {
            Field<std::string> asset;
            Field<float> progress;

            template <typename Visitor>
            void visitFields(Visitor& v) {
                v(asset, "asset", "asset id");
                v(progress, "progress", "progress 0-1");
            }
        };
    };

    using Ops = std::tuple<OpMakeAssetsAvailable>;
    using Ins = std::tuple<>;
    using Outs = std::tuple<StreamProgress>;
};

struct Interface {
    static inline constexpr std::string_view id = "acord";
    static inline constexpr std::string_view desc = "Acord interface";

    using States = std::tuple<State>;
};

} // namespace acord

} // namespace ac::schema
