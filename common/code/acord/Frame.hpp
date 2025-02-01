// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <vector>
#include <string>
#include <cstdint>

namespace acord {

struct Frame {
    // if text is not empty, data is assumed to be text and binary is disregarded
    // if text is empty, data is assumed to be binary (even if binary is empty)
    std::string textBuf;
    std::vector<uint8_t> binaryBuf;

    bool text() const noexcept {
        return !textBuf.empty();
    }

    bool binary() const noexcept {
        return !text();
    }

    size_t size() const noexcept {
        return text() ? textBuf.size() : binaryBuf.size();
    }

    bool empty() const noexcept {
        return textBuf.empty() && binaryBuf.empty();
    }
};

} // namespace acord
