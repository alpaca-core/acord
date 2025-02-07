// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <astl/symbol_export.h>

#if ACORD_SERVER_SHARED
#   if BUILDING_ACORD_SERVER
#       define ACORD_SERVER_API SYMBOL_EXPORT
#   else
#       define ACORD_SERVER_API SYMBOL_IMPORT
#   endif
#else
#   define ACORD_SERVER_API
#endif
