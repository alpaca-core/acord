// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <vector>
#include <string>

namespace acord {

struct Frame {
    // if text is not empty, data is assumed to be text and binary is disregarded
    // if text is empty, data is assumed to be binary (even if binary is empty)
    std::string textBuffer;
    std::vector<uint8_t> binaryBuffer;

    bool text() const noexcept {
        return !textBuffer.empty();
    }

    bool binary() const noexcept {
        return !text();
    }

    size_t size() const noexcept {
        return text() ? textBuffer.size() : binaryBuffer.size();
    }

    bool empty() const noexcept {
        return textBuffer.empty() && binaryBuffer.empty();
    }
};

} // namespace acord
