// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <ac/jalog/Scope.hpp>
#include <ac/jalog/Log.hpp>

namespace acord::asset::log {
extern ac::jalog::Scope scope;
}

#define ACORD_ASSET_LOG(lvl, ...) AC_JALOG_SCOPE(::acord::asset::log::scope, lvl, __VA_ARGS__)
