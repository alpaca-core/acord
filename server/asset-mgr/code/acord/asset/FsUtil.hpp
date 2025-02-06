// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "api.h"
#include <string>
#include <cstdint>
#include <string_view>

namespace acord::asset::fs {

ACORD_ASSET_MGR_API std::string getTempDir();

ACORD_ASSET_MGR_API std::string getFileName(std::string_view path);

ACORD_ASSET_MGR_API void expandPathInPlace(std::string& path); // expands in place
ACORD_ASSET_MGR_API std::string expandPath(std::string_view path);

struct BasicStat {
    enum Type {
        NotExist,
        File,
        Directory,
        Other
    } type;
    uint64_t size;

    bool exists() const noexcept { return type != NotExist; }
    bool file() const noexcept { return type == File; }
    bool dir() const noexcept { return type == Directory; }
    bool other() const noexcept { return type == Other; }
};

ACORD_ASSET_MGR_API BasicStat basicStat(const std::string& path) noexcept;

ACORD_ASSET_MGR_API bool mkdir_p(std::string_view path);
ACORD_ASSET_MGR_API bool rm_r(std::string_view path, bool f = false);
ACORD_ASSET_MGR_API void touch(const std::string& path, bool mkdir = true);

ACORD_ASSET_MGR_API void mv(std::string_view src, std::string_view tgt);

} // namespace ac::asset::fs
