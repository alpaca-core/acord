// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "AssetMgr.hpp"
#include "FsUtil.hpp"
#include "Logging.hpp"

#include <ac/frameio/local/BufferedChannel.hpp>
#include <ac/frameio/local/BufferedChannelStream.hpp>
#include <ac/frameio/StreamEndpoint.hpp>
#include <ac/frameio/local/BlockingIo.hpp>

#include <ahttp/ahttp.hpp>
#include <astl/move_capture.hpp>

#include <ac/xec/context.hpp>
#include <ac/xec/context_work_guard.hpp>
#include <ac/xec/post.hpp>

#include <thread>
#include <fstream>
#include <unordered_map>

namespace acord::server {

struct AssetMgr::Impl {
    ac::frameio::BlockingIoCtx m_blockingCtx;
    ac::xec::context m_xctx;
    ac::xec::strand m_strand{m_xctx.make_strand()};
    std::thread m_thread;

    std::vector<std::string> m_localDirs;
    std::unordered_map<std::string, std::string> m_assetUrlToPath;

    Impl() {
        m_localDirs.push_back(fs::expandPath("~/.ac-assets"));
        fs::mkdir_p(m_localDirs.front());

        m_thread = std::thread([this]() {
            auto guard = m_xctx.make_work_guard();
            m_xctx.run();
        });
    }

    ~Impl() {
        m_xctx.stop();
        m_thread.join();
    }

    void makeAssetsAvailable(std::vector<std::string>& assetUrls, ac::frameio::StreamEndpoint& ep) {
        std::vector<std::string> paths(assetUrls.size());

        ac::frameio::BlockingIo io(std::move(ep), m_blockingCtx);

        for (int i = 0; i < assetUrls.size(); ++i) {
            auto& url = assetUrls[i];
            auto& path = paths[i];

            // look for assets in local cache
            auto it = m_assetUrlToPath.find(url);
            if (it != m_assetUrlToPath.end()) {
                ACORD_SRV_LOG(Info, "Found asset '", url, "' in cache: ", it->second);
                path = it->second;
                continue;
            }

            // look in local dirs
            bool found = false;
            auto fname = fs::getFileName(url);
            for (auto& dir : m_localDirs) {
                auto localPath = dir + "/" + fname;
                if (fs::basicStat(localPath).file()) {
                    ACORD_SRV_LOG(Info, "Found asset '", url, "' in local dir: ", localPath);
                    m_assetUrlToPath[url] = localPath;
                    path = localPath;
                    found = true;
                    break;
                }
            }
            if (found) continue;

            // download from remote
            auto dlPath = fs::getTempDir() + "/acord-dl/" + fname;
            fs::touch(dlPath);

            std::ofstream ofs(dlPath, std::ios::binary);
            if (!ofs) {
                ACORD_SRV_LOG(Error, "Failed to open file for writing: ", dlPath);
                continue;
            }

            try {
                auto gen = ahttp::get_sync(url);
                std::vector<uint8_t> buf(1024 * 1024);

                int tmb = 0;
                while (!gen.done()) {
                    auto chunk = gen.get_next_chunk(buf);
                    ofs.write((const char*)chunk.data(), chunk.size());
                    ++tmb;
                    if (tmb % 10 == 0) {
                        ACORD_SRV_LOG(Info, tmb/10, "0 MB downloading ", url);
                    }
                }
                ofs.close();

                ACORD_SRV_LOG(Info, "Downloaded asset '", url, "' to: ", dlPath);

                auto targetPath = m_localDirs.front() + "/" + fname;
                fs::mv(dlPath, targetPath);

                ACORD_SRV_LOG(Info, "Moved temp file '", dlPath, "' to: ", targetPath);

                m_assetUrlToPath[url] = targetPath;
                path = targetPath;
            }
            catch (std::exception& e) {
                ACORD_SRV_LOG(Error, "Failed to download asset '", url, "': ", e.what());
            }
        }

        io.push({.op = "assets", .data = std::move(paths)});
    }
};

AssetMgr::AssetMgr() : m_impl(std::make_unique<Impl>()) {}
AssetMgr::~AssetMgr() = default;

ac::frameio::StreamEndpoint AssetMgr::makeAssetsAvailable(std::vector<std::string> assetUrls) {
    auto [local, remote] = ac::frameio::BufferedChannel_getEndpoints(
        ac::frameio::BufferedChannel_create(5),
        ac::frameio::BufferedChannel_create(5)
    );

    post(m_impl->m_strand, [this, movecap(assetUrls, local)]() mutable {
        m_impl->makeAssetsAvailable(assetUrls, local);
    });

    return std::move(remote);
}

} // namespace acord::server
