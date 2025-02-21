// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "api.h"
#include "FrameCvt.hpp"
#include <ac/frameio/StreamEndpoint.hpp>
#include <fishnets/util/WsSessionHandler.hpp>
#include <optional>

namespace acord {

class ACORD_COMMON_API CommonWsSession : public fishnets::WsSessionHandler {
protected:
    ~CommonWsSession();
    FrameCvt m_cvt;
    ac::frameio::StreamEndpoint m_dispatch;

    std::optional<ac::Frame> m_receiving;
    std::optional<acord::Frame> m_sending;

    // not final (likely to be overridden further)
    void wsOpened(std::string_view target) override;
    void wsClosed(std::string reason) override;

    void tryWriteToDispatch();

    template <typename T>
    void received(itlib::span<T> span);

    void wsReceivedText(itlib::span<char> text, bool complete) final override;
    void wsReceivedBinary(itlib::span<uint8_t> binary, bool complete) final override;
    void tryReadFromDispatch();
    void doSend();
    void wsCompletedSend() final override;
};

} // namespace acord
