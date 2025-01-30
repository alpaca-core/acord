// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "Frame.hpp"
#include <ac/Frame.hpp>
#include <span>

namespace acord {

namespace cvt {
static constexpr std::string_view Key_op = "op";
static constexpr std::string_view Key_data = "data";

inline ac::Dict acToJson(ac::Frame& in) {
    ac::Dict json;
    json[Key_op] = std::move(in.op);
    json[Key_data] = std::move(in.data);
    return json;
}
inline void jsonToAc(ac::Frame& out, ac::Dict& in) {
    out.op = std::move(in[Key_op].get_ref<std::string&>());
    out.data = std::move(in[Key_data]);
}

}

struct FrameCvt {
    void (*fromAcFrame)(Frame& out, ac::Frame& in) = nullptr;

    static void jsonBufFromAc(std::string& out, ac::Frame& in) {
        auto json = cvt::acToJson(in);
        out = json.dump();
    }

    static void jsonBufToAc(ac::Frame& out, std::string_view in) {
        auto json = ac::Dict::parse(in);
        cvt::jsonToAc(out, json);
    }

    static void cborBufFromAc(std::vector<uint8_t>& out, ac::Frame& in) {
        auto json = cvt::acToJson(in);
        out = ac::Dict::to_cbor(json);
    }

    static void cborBufToAc(ac::Frame& out, std::span<uint8_t> in) {
        auto json = ac::Dict::from_cbor(in);
        cvt::jsonToAc(out, json);
    }

    static void jsonFrameFromAc(Frame& out, ac::Frame& in) {
        jsonBufFromAc(out.textBuf, in);
    }

    static void cborFrameFromAc(Frame& out, ac::Frame& in) {
        cborBufFromAc(out.binaryBuf, in);
    }

    static FrameCvt json() {
        return {jsonFrameFromAc};
    }
    static FrameCvt cbor() {
        return {cborFrameFromAc};
    }
};

} // namespace acord
