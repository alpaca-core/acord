// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include "Source.hpp"
#include <string>
#include <memory>

namespace ac::asset {
AC_ASSET_EXPORT std::unique_ptr<Source> SourceLocalDir_Create(std::string path);
}
