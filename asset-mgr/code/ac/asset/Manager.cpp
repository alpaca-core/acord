// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "Manager.hpp"
#include "Source.hpp"

#include <xec/TaskExecutor.hpp>
#include <xec/ThreadExecution.hpp>
#include <xec/ThreadName.hpp>

#include <astl/move_capture.hpp>
#include <astl/move.hpp>
#include <astl/tsumap.hpp>
#include <itlib/flat_map.hpp>

namespace ac::asset {

class Manager::Impl {
    itlib::flat_map<int, std::vector<std::unique_ptr<Source>>> m_sources;

    astl::tsumap<Info> m_assets;

    // these must the last members (first to be destroyed)
    // if there are pending tasks, they will be finalized here and they may access other members
    xec::TaskExecutor m_executor;
    xec::LocalExecution m_execution;
    std::thread m_thread;

    auto getAssetInfo(std::string& id) {
        auto f = m_assets.find(id);
        if (f != m_assets.end()) {
            return f;
        }
        for (auto& [_, sources] : m_sources) {
            for (auto& source : sources) {
                auto basicInfo = source->checkAssetSync(id);
                if (!basicInfo) continue;
                auto ret = m_assets.try_emplace(m_assets.end(), id, Info{
                    .source = source.get(),
                    .size = basicInfo->size,
                    .path = astl::move(basicInfo->path),
                });
                return ret;
            }
        }
        return m_assets.end();
    }
public:
    Impl(uint32_t flags) : m_execution(m_executor) {
        if (!(flags & No_LaunchThread)) {
            launchThread();
        }
    }

    ~Impl() {
        // now we could always call abortRun() here, but that has the potential to hide misuse
        // so to increase the likelihood of a crash, we do it only if we own the execution
        if (m_thread.joinable()) {
            abortRun();
            m_thread.join();
        }
    }

    void launchThread() {
        m_thread = std::thread([this]() {
            xec::SetThisThreadName("ac-assets");
            run();
        });
    }

    void queryAsset(std::string id, QueryAssetCb cb) {
        m_executor.pushTask([this, movecap(cb, id)]() mutable {
            auto f = getAssetInfo(id);
            if (f != m_assets.end()) {
                return cb(f->first, f->second);
            }
            cb(id, {.error = "Asset not found"});
        });
    }

    void getAsset(std::string id, GetAssetCb cb, GetAssetProgressCb progressCb) {
        m_executor.pushTask([this, movecap(cb, id, progressCb)]() mutable {
            auto f = getAssetInfo(id);
            if (f == m_assets.end()) {
                return cb(id, {.error = "Can't get asset. No source"});
            }
            auto& info = f->second;
            if (info.path) {
                return cb(f->first, info);
            }
            try {
                auto res = info.source->fetchAssetSync(id, [&](float p) {
                    return progressCb(id, p);
                });

                info.size = res.size;
                info.path = astl::move(res.path);
            }
            catch (std::exception& ex) {
                info.error = ex.what();
            }
            return cb(f->first, info);
        });
    }

    void addSource(std::unique_ptr<Source> source, int priority) {
        m_executor.pushTask([this, movecap(source), priority]() mutable {
            m_sources[priority].push_back(astl::move(source));
        });
    }

    void abortRun() {
        m_executor.stop();
    }

    void pushStop() {
        m_executor.pushTask([this]() {
            m_executor.stop();
        });
    }

    void run() {
        m_execution.run();
    }
};

Manager::Manager(uint32_t flags) : m_impl(std::make_unique<Impl>(flags)) {}
Manager::~Manager() = default;

void Manager::queryAsset(std::string id, QueryAssetCb cb) {
    m_impl->queryAsset(astl::move(id), astl::move(cb));
}

void Manager::getAsset(std::string id, GetAssetCb cb, GetAssetProgressCb progressCb) {
    m_impl->getAsset(astl::move(id), astl::move(cb), astl::move(progressCb));
}

void Manager::addSource(std::unique_ptr<Source> source, int priority) {
    m_impl->addSource(astl::move(source), priority);
}

void Manager::run() {
    m_impl->run();
}

void Manager::abortRun() {
    m_impl->abortRun();
}

void Manager::pushStop() {
    m_impl->pushStop();
}

Source::~Source() = default; // export vtable

} // namespace ac::asset
