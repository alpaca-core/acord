// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <splat/symbol_export.h>

#if AC_ASSET_SHARED
#   if BUILDING_AC_ASSET
#       define AC_ASSET_EXPORT SYMBOL_EXPORT
#   else
#       define AC_ASSET_EXPORT SYMBOL_IMPORT
#   endif
#else
#   define AC_ASSET_EXPORT
#endif
