// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "Frame.hpp"
#include <ac/Frame.hpp>

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

struct FrameCvtJson {
    static void to(ac::Frame& out, Frame& in) {
        auto json = ac::Dict::parse(in.textBuffer);
        cvt::jsonToAc(out, json);
    }
    static void from(Frame& out, ac::Frame& in) {
        auto json = cvt::acToJson(in);
        out.textBuffer = json.dump();
    }
};

struct FrameCvtCbor {
    static void to(ac::Frame& out, Frame& in) {
        auto json = ac::Dict::from_cbor(in.binaryBuffer);
        cvt::jsonToAc(out, json);
    }
    static void from(Frame& out, ac::Frame& in) {
        auto json = cvt::acToJson(in);
        out.binaryBuffer = ac::Dict::to_cbor(json);
    }
};

struct FrameCvt {
    void (*toAcFrame)(ac::Frame& out, Frame& in) = nullptr;
    void (*fromAcFrame)(Frame& out, ac::Frame& in) = nullptr;

    static FrameCvt json() {
        return {FrameCvtJson::to, FrameCvtJson::from};
    }
    static FrameCvt cbor() {
        return {FrameCvtCbor::to, FrameCvtCbor::from};
    }
};

} // namespace acord
