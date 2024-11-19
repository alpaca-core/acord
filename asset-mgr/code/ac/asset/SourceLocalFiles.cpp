// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "SourceLocalFiles.hpp"
#include "FsUtil.hpp"
#include <nlohmann/json.hpp>
#include <astl/throw_stdex.hpp>
#include <stdexcept>

namespace ac::asset {

SourceLocalFiles::SourceLocalFiles(std::string_view id, const astl::tsumap<std::string>& manifest)
    : m_id(id)
{
    // fill available assets
    for (auto& [i, v] : manifest) {
        auto path = fs::expandPath(v);
        auto st = fs::basicStat(path);

        auto& entry = m_assetManifest[i];
        if (st.file()) {
            entry.info = {st.size, std::move(path)};
        }
        entry.path = v;
    }
}

namespace {
astl::tsumap<std::string> parseManifest(std::string_view jsonManifest) {
    astl::tsumap<std::string> result;
    auto manifest = nlohmann::json::parse(jsonManifest);
    for (const auto& i : manifest.items()) {
        result[i.key()] = i.value().get<std::string>();
    }
    return result;
}
} // namespace


SourceLocalFiles::SourceLocalFiles(std::string_view id, std::string_view jsonManifest)
    : SourceLocalFiles(id, parseManifest(jsonManifest))
{}

std::optional<Source::BasicAssetInfo> SourceLocalFiles::checkAssetSync(std::string_view id) noexcept {
    if (auto it = m_assetManifest.find(id); it != m_assetManifest.end()) {
        return it->second.info;
    }
    return std::nullopt;
}

Source::BasicAssetInfo SourceLocalFiles::fetchAssetSync(std::string_view id, ProgressCb) {
    if (auto it = m_assetManifest.find(id); it != m_assetManifest.end()) {
        if (it->second.info.path) {
            return it->second.info;
        }
        else {
            throw_ex{} << "File not found: " << it->second.path;
        }
    }
    else {
        throw_ex{} << "Asset not found: " << id;
    }
}

} // namespace ac::asset
