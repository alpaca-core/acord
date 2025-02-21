// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "CommonWsSession.hpp"
#include "WsSessionHandlerWobj.hpp"

#include <ac/jalog/Log.hpp>

namespace acord {

CommonWsSession::~CommonWsSession() = default;

void CommonWsSession::wsOpened(std::string_view target) {
    if (target == "/cbor") {
        m_cvt = FrameCvt::cbor();
    }
    else {
        m_cvt = FrameCvt::json();
    }

    wsReceive();
}

void CommonWsSession::wsClosed(std::string reason) {
    m_dispatch.read_stream->close();
    m_dispatch.write_stream->close();
}

void CommonWsSession::tryWriteToDispatch() {
    if (!m_receiving) {
        return;
    }
    auto res = m_dispatch.write_stream->write(*m_receiving, [&] {
        return [this, pl = shared_from_this()] {
            postWsIoTask([this]() {
                tryWriteToDispatch();
            });
        };
    });
    if (res.blocked()) return;
    if (res.closed()) {
        wsClose();
        return;
    }
    assert(res.success());
    m_receiving.reset();
    wsReceive();
}

template <typename T>
void CommonWsSession::received(itlib::span<T> span) {
    assert(!m_receiving);
    auto& frame = m_receiving.emplace();

    try {
        if constexpr (std::is_same_v<T, char>) {
            m_cvt.jsonBufToAc(frame, {span.data(), span.size()});
        }
        else {
            m_cvt.cborBufToAc(frame, {span.data(), span.size()});
        }
    }
    catch (std::exception& ex) {
        AC_JALOG(Error, "Error reading frame: ", ex.what());
        wsClose();
        return;
    }

    tryWriteToDispatch();
}

void CommonWsSession::wsReceivedText(itlib::span<char> text, bool) {
    received(text);
}

void CommonWsSession::wsReceivedBinary(itlib::span<uint8_t> binary, bool) {
    received(binary);
}

void CommonWsSession::tryReadFromDispatch() {
    if (m_sending) return;
    auto& buf = m_sending.emplace();

    ac::Frame frame;
    auto res = m_dispatch.read_stream->read(frame, [this] {
        return [this, pl = shared_from_this()] {
            postWsIoTask([this]() {
                tryReadFromDispatch();
            });
        };
    });

    if (res.success()) {
        m_cvt.fromAcFrame(buf, frame);
        doSend();
        return;
    }

    m_sending.reset();

    if (res.closed()) {
        wsClose();
    }

    assert(res.closed() || res.blocked());
}

void CommonWsSession::doSend() {
    assert(!!m_sending);
    if (m_sending->text()) {
        wsSend(m_sending->textBuf);
    }
    else {
        wsSend(m_sending->binaryBuf);
    }
}

void CommonWsSession::wsCompletedSend() {
    m_sending.reset();
    tryReadFromDispatch();
}

} // namespace acord
