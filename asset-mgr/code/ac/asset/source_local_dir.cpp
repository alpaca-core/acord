// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "source_local_dir.h"
#include "SourceLocalDir.hpp"

ac_asset_source* ac_new_asset_source_local_dir(const char* path) {
    return reinterpret_cast<ac_asset_source*>(ac::asset::SourceLocalDir_Create(path).release());
}
