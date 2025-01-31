// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "api.h"
#include "FrameCvt.hpp"
#include <ac/frameio/StreamEndpoint.hpp>
#include <fishnets/WebSocketSession.hpp>
#include <astl/shared_from.hpp>

namespace acord {

class ACORD_COMMON_EXPORT CommonWsSession : public fishnets::WebSocketSession, public astl::enable_shared_from {
protected:
    ~CommonWsSession();
    FrameCvt m_cvt = FrameCvt::json();
    ac::frameio::StreamEndpoint m_dispatch;
};

} // namespace acord
