// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once

namespace ac {
namespace xec { class context; }
namespace local { class IoCtx; }
}

namespace acord::server {
class AssetMgr;
struct AppCtx {
    ac::xec::context& localSessionXCtx;
    ac::local::IoCtx& ioCtx;
    AssetMgr& assetMgr;
};
} // namespace acord::server
