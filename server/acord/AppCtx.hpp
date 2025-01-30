// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once

namespace ac::frameio {
class LocalIoCtx;
}

namespace acord::server {
struct LocalSessionFactory;

struct AppCtx {
    ac::frameio::LocalIoCtx& io;
    LocalSessionFactory& sessionFactory;
};
} // namespace acord::server
