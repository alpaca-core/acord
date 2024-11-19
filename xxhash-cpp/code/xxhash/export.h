// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <splat/symbol_export.h>

#if XXHASH_CPP_SHARED
#   if BUILDING_XXHASH_CPP
#       define XXHASH_CPP_EXPORT SYMBOL_EXPORT
#   else
#       define XXHASH_CPP_EXPORT SYMBOL_IMPORT
#   endif
#else
#   define XXHASH_CPP_EXPORT
#endif
