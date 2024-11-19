// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <splat/symbol_export.h>

#if AHTTP_SHARED
#   if BUILDING_AHTTP
#       define AHTTP_EXPORT SYMBOL_EXPORT
#   else
#       define AHTTP_EXPORT SYMBOL_IMPORT
#   endif
#else
#   define AHTTP_EXPORT
#endif
