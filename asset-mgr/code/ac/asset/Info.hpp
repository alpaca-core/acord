// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <optional>
#include <cstddef>
#include <string>

namespace ac::asset {
class Source;
struct Info {
    Source* source = nullptr; // asset source if there is one (without a source it can't be fetched)
    std::optional<size_t> size; // may not be available until fetched
    std::optional<std::string> path; // path on disk if available (otherwise asset has not been fetched, #53)
    std::optional<std::string> error; // error message if asset is not available or fetch failed
};
} // namespace ac::asset
