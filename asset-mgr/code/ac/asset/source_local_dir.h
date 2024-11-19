// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include "source.h"

#if defined(__cplusplus)
extern "C" {
#endif

AC_ASSET_EXPORT ac_asset_source* ac_new_asset_source_local_dir(const char* path);

#if defined(__cplusplus)
}
#endif
