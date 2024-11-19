// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <ac/jalog/Scope.hpp>
#include <ac/jalog/Log.hpp>

namespace ac::asset::log {
extern ac::jalog::Scope scope;
}

#define AC_ASSET_LOG(lvl, ...) AC_JALOG_SCOPE(::ac::asset::log::scope, lvl, __VA_ARGS__)
