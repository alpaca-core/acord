// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once

namespace ac::frameio {
class LocalIoCtx;
}
struct LocalSessionFactory;

struct AppCtx {
    ac::frameio::LocalIoCtx& io;
    LocalSessionFactory& sessionFactory;
};