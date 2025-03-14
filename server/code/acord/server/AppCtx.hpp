// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once

namespace ac {
namespace xec { class context; }
namespace local { class Backend; }
}

namespace acord::server {
struct AppCtx {
    ac::xec::context& localSessionXCtx;
    ac::local::Backend& backend;
};
} // namespace acord::server
