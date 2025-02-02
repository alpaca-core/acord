// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "api.h"
#include "FrameCvt.hpp"
#include <ac/frameio/StreamEndpoint.hpp>
#include <fishnets/WebSocketSession.hpp>
#include <astl/shared_from.hpp>
#include <deque>
#include <optional>

namespace acord {

class ACORD_COMMON_EXPORT CommonWsSession : public fishnets::WebSocketSession, public astl::enable_shared_from {
protected:
    ~CommonWsSession();
    FrameCvt m_cvt = FrameCvt::json();
    ac::frameio::StreamEndpoint m_dispatch;

    std::deque<ac::Frame> m_received;
    std::optional<acord::Frame> m_sending;

    void wsClosed() override; // not final (likely to be overridden further)

    void tryWriteToDispatch();

    template <typename T>
    void received(itlib::span<T> span);

    void wsReceivedText(itlib::span<char> text) final override;
    void wsReceivedBinary(itlib::span<uint8_t> binary) final override;
    void tryReadFromDispatch();
    void doSend();
    void wsCompletedSend() final override;
};

} // namespace acord
