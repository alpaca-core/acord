// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "api.h"

namespace ac::local {
class ServiceFactory;
}

namespace acord::server {
ACORD_SERVER_API ac::local::ServiceFactory& AssetMgr_getServiceFactory();
} // namespace acord::server
