// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <astl/symbol_export.h>

#if ACORD_COMMON_SHARED
#   if BUILDING_ACORD_COMMON
#       define ACORD_COMMON_API SYMBOL_EXPORT
#   else
#       define ACORD_COMMON_API SYMBOL_IMPORT
#   endif
#else
#   define ACORD_COMMON_API
#endif
