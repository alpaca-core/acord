// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "xxh-config.h"
#include "export.h"

#include <span>
#include <string_view>
#include <type_traits>

namespace xxhash {

enum class type {
    xxh64,
    xxh128,
};

template <type>
struct hasher_traits;

template <>
struct hasher_traits<type::xxh64> {
    using state_t = XXH64_state_t;
    using hash_t = XXH64_hash_t;
};

struct xxh128_hash_type : public XXH128_hash_t {
    xxh128_hash_type() = default;
    xxh128_hash_type(uint64_t low, uint64_t high) : XXH128_hash_t{low, high} {}
    xxh128_hash_type(const XXH128_hash_t& src) : XXH128_hash_t{src} {}
    bool operator==(const xxh128_hash_type& other) const {
        return low64 == other.low64 && high64 == other.high64;
    }
};

template <>
struct hasher_traits<type::xxh128> {
    using state_t = XXH3_state_t;
    using hash_t = xxh128_hash_type;
};

template <type XXHtype>
class hasher {
    using state_t = typename hasher_traits<XXHtype>::state_t;
    state_t m_state;

    template <typename T>
    static std::span<const uint8_t> make_span(const T& t) {
        if constexpr (std::is_convertible_v<T, std::string_view>) {
            std::string_view str(t);
            return std::span(reinterpret_cast<const uint8_t*>(str.data()), str.length());
        }
        else {
            static_assert(std::is_trivial<T>::value, "Attempting to hash a non-trivial type");
            return std::span(reinterpret_cast<const uint8_t*>(&t), sizeof(t));
        }
    }
public:
    using hash_t = typename hasher_traits<XXHtype>::hash_t;

    static hash_t hash(std::string_view str, uint64_t seed = 0) { return ihash(str.data(), str.length(), seed); }

    template <typename T>
    static hash_t hash(std::span<T> v, uint64_t seed = 0) {
        static_assert(std::is_trivial_v<std::remove_const_t<T>>, "Attempting to hash a non-trivial type");
        return ihash(v.data(), v.size(), seed);
    }

    template <typename T>
    static hash_t hash(const T& v, uint64_t seed = 0) {
        return hash(make_span(v), seed);
    }

    explicit hasher(uint64_t seed = 0) { reset(seed); }
    void reset(uint64_t seed = 0);

    template <typename T>
    void update(std::span<T> v) {
        static_assert(std::is_trivial_v<std::remove_const_t<T>>, "Attempting to hash a non-trivial type");
        iupdate(v.data(), v.size());
    }

    template <typename T>
    void update(const T& v) {
        update(make_span(v));
    }

    void update(const void* ptr, size_t size) {
        update(std::span(reinterpret_cast<const uint8_t*>(ptr), size));
    }

    hash_t digest() const;

private:
    static hash_t ihash(const void* ptr, size_t size, uint64_t seed = 0);
    void iupdate(const void* ptr, size_t size);
};

using h64 = hasher<type::xxh64>;
using h128 = hasher<type::xxh128>;

} // namespace xxhash
