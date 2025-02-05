// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include "sync_generator.hpp"
#include <string_view>

namespace ahttp {

AHTTP_EXPORT bool supports_https() noexcept;
AHTTP_EXPORT bool supports_url(std::string_view url) noexcept;

// initiate a synchronous file download
// returns a generator that yields chunks of the file
AHTTP_EXPORT sync_generator get_sync(std::string_view url);

} // namespace ahttp
