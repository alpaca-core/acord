// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <ac/frameio/SessionHandlerPtr.hpp>

struct LocalSessionFactory {
    ac::frameio::SessionHandlerPtr createHandler();
};

