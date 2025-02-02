// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "CommonWsSession.hpp"

#include <ac/jalog/Log.hpp>

namespace acord {

CommonWsSession::~CommonWsSession() = default;

void CommonWsSession::wsClosed() {
    AC_JALOG(Info, "Session closed");
    m_dispatch.readStream->close();
    m_dispatch.writeStream->close();
}

void CommonWsSession::tryWriteToDispatch() {
    while (true) {
        if (m_received.empty()) {
            return;
        }
        auto res = m_dispatch.writeStream->write(m_received.front(), [&] {
            return [this, pl = shared_from_this()] {
                postWSIOTask([this]() {
                    tryWriteToDispatch();
                });
            };
        });
        if (res.blocked()) break;
        if (res.closed()) {
            wsClose();
            return;
        }
        assert(res.success());
        m_received.pop_front();
    }
}

template <typename T>
void CommonWsSession::received(itlib::span<T> span) {
    auto& frame = m_received.emplace_back();

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

    if (m_received.size() == 1) {
        tryWriteToDispatch();
    }
}

void CommonWsSession::wsReceivedText(itlib::span<char> text) {
    received(text);
}

void CommonWsSession::wsReceivedBinary(itlib::span<uint8_t> binary) {
    received(binary);
}

void CommonWsSession::tryReadFromDispatch() {
    if (m_sending) return;
    auto& buf = m_sending.emplace();

    ac::Frame frame;
    auto res = m_dispatch.readStream->read(frame, [this] {
        return [this, pl = shared_from_this()] {
            postWSIOTask([this]() {
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
