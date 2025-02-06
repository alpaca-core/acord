// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <astl/symbol_export.h>

#if ACORD_ASSET_MGR_SHARED
#   if BUILDING_ACORD_ASSET_MGR
#       define ACORD_ASSET_MGR_API SYMBOL_EXPORT
#   else
#       define ACORD_ASSET_MGR_API SYMBOL_IMPORT
#   endif
#else
#   define ACORD_ASSET_MGR_API
#endif
