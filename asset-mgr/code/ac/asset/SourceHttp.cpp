// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "SourceHttp.hpp"
#include "FsUtil.hpp"
#include <ahttp/ahttp.hpp>
#include <xxhash/hash.hpp>
#include <astl/move.hpp>
#include <astl/throw_stdex.hpp>
#include <fstream>

namespace ac::asset {

SourceHttp::SourceHttp(std::string id, std::string edownloadDir)
    : m_id(astl::move(id))
    , m_downloadDir(astl::move(edownloadDir))
{
    fs::expandPathInPlace(m_downloadDir);
}

void SourceHttp::addAsset(std::string id, ManifestEntry entry) {
    AssetManifestEntry ame;
    (ManifestEntry&)ame = astl::move(entry);
    ame.targetPath = m_downloadDir + "/" + id;

    auto st = fs::basicStat(ame.targetPath);
    if (st.file()) {
        ame.info.size = st.size;
        ame.info.path = ame.targetPath;
    }
    else if (st.exists()) {
        throw_ex{} << "Target path is not a file: " << ame.targetPath;
    }

    m_assetManifest.emplace(std::move(id), std::move(ame));
}

std::optional<Source::BasicAssetInfo> SourceHttp::checkAssetSync(std::string_view id) noexcept {
    auto it = m_assetManifest.find(std::string(id));
    if (it == m_assetManifest.end()) {
        return std::nullopt;
    }
    return it->second.info;
}

Source::BasicAssetInfo SourceHttp::fetchAssetSync(std::string_view id, ProgressCb pcb) {
    auto it = m_assetManifest.find(std::string(id));
    if (it == m_assetManifest.end()) {
        throw_ex{} << "Asset not found: " << id;
    }

    auto& ame = it->second;

    if (ame.info.path) {
        // already downloaded
        return ame.info;
    }

    auto g = ahttp::get_sync(ame.url);
    auto fout = std::ofstream(ame.targetPath, std::ios::binary);
    if (!fout) {
        throw_ex{} << "Failed to open file for writing: " << ame.targetPath;
    }

     // set info size here, thus even if we abort, we will have the correct size available
    ame.info.size = g.size();

    bool continueDownload = pcb(0);

    static constexpr size_t chunkSize = 1024 * 1024; // 1mb chunks
    std::vector<uint8_t> buf(chunkSize);
    size_t totalDownloaded = 0;
    xxhash::h64 hasher;
    while (!g.done()) {
        if (!continueDownload) {
            // return incomplete info (abort is not an error)
            return ame.info;
        }

        auto chunk = g.get_next_chunk(buf);
        fout.write((const char*)chunk.data(), chunk.size());
        hasher.update(chunk);
        totalDownloaded += chunk.size();

        if (g.size()) {
            // if we have a size, we can report meaningful progress
            continueDownload = pcb(float(totalDownloaded) / *g.size());
        }
        else {
            // otherwise report zero, but still keep asking if we should continue
            continueDownload = pcb(0);
        }
    }
    fout.close();

    if (ame.xxhash && *ame.xxhash != hasher.digest()) {
        throw_ex{} << "Hash mismatch for asset: " << id;
    }

    ame.info.size = totalDownloaded;
    ame.info.path = ame.targetPath;
    return ame.info;
}

} // namespace ac::asset
