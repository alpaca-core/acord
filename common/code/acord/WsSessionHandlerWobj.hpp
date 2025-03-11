// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <ac/xec/wait_func.hpp>
#include <ac/xec/wait_func_invoke.hpp>
#include <fishnets/ExecutorPtr.hpp>
#include <fishnets/Post.hpp>

namespace acord {

class WsSessionHandlerWobj {
    fishnets::ExecutorPtr m_executor;
    ac::xec::wait_func m_cb;
public:
    explicit WsSessionHandlerWobj(fishnets::ExecutorPtr e) : m_executor(std::move(e)) {}

    void notify_one() {
        post(m_executor, [this] {
            if (m_cb) {
                auto cb = std::exchange(m_cb, nullptr);
                ac::xec::wait_func_invoke_cancelled(cb);
            }
        });
    }

    void wait(ac::xec::wait_func cb) {
        if (m_cb) {
            post(m_executor, [old = std::move(m_cb)] {
                ac::xec::wait_func_invoke_cancelled(old);
            });
        }
        m_cb = std::move(cb);
    }

    using executor_type = fishnets::ExecutorPtr;
    const fishnets::ExecutorPtr& get_executor() const {
        return m_executor;
    }

    //// corouitne interface implemented in coro_wobj.hpp
    //[[nodiscard]] wait_awaitable<strand_wobj> wait() {
    //    return wait_awaitable(*this);
    //}
};

} // namespace ac::xec
