// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#define XXH_IMPLEMENTATION
#include "xxh-config.h"
#include "hash.hpp"

namespace xxhash {

template <> XXHASH_CPP_EXPORT
hasher<type::xxh64>::hash_t hasher<type::xxh64>::ihash(const void* ptr, size_t size, uint64_t seed) {
    return XXH64(ptr, size, seed);
}
template <> XXHASH_CPP_EXPORT
void hasher<type::xxh64>::reset(uint64_t seed) {
    XXH64_reset(&m_state, seed);
}
template <> XXHASH_CPP_EXPORT
void hasher<type::xxh64>::iupdate(const void* ptr, size_t size) {
    XXH64_update(&m_state, ptr, size);
}
template <> XXHASH_CPP_EXPORT
hasher<type::xxh64>::hash_t hasher<type::xxh64>::digest() const {
    return XXH64_digest(&m_state);
}

template <> XXHASH_CPP_EXPORT
hasher<type::xxh128>::hash_t hasher<type::xxh128>::ihash(const void* ptr, size_t size, uint64_t seed) {
    return XXH3_128bits_withSeed(ptr, size, seed);
}
template <> XXHASH_CPP_EXPORT
void hasher<type::xxh128>::reset(uint64_t seed) {
    XXH3_128bits_reset_withSeed(&m_state, seed);
}
template <> XXHASH_CPP_EXPORT
void hasher<type::xxh128>::iupdate(const void* ptr, size_t size) {
    XXH3_128bits_update(&m_state, ptr, size);
}
template <> XXHASH_CPP_EXPORT
hasher<type::xxh128>::hash_t hasher<type::xxh128>::digest() const {
    return XXH3_128bits_digest(&m_state);
}

} // namespace xxhash
