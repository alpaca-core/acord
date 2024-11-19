// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once

// our configuration of xxhash
#define XXH_STATIC_LINKING_ONLY
#if !defined(NDEBUG)
#define XXH_DEBUGLEVEL 1
#endif

#include <xxhash.h>
