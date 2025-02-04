// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <astl/symbol_export.h>

#if ACORD_CLIENT_SHARED
#   if BUILDING_ACORD_CLIENT
#       define ACORD_CLIENT_API SYMBOL_EXPORT
#   else
#       define ACORD_CLIENT_API SYMBOL_IMPORT
#   endif
#else
#   define ACORD_CLIENT_API
#endif
