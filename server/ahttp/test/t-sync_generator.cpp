// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <ahttp/sync_generator.hpp>
#include <random>
#include <stdexcept>
#include <doctest/doctest.h>

enum test_type {
    no_throw,
    throw_on_connect,
    throw_before_chunks,
    throw_on_first_chunk,
    throw_on_second_chunk,
    throw_on_disconnect,
};

constexpr uint32_t seed = 42;
constexpr size_t dsize = 1024;

ahttp::sync_generator test(test_type t) {
    std::vector<uint8_t> data(dsize);
    {
        std::minstd_rand gen(seed);
        for (auto& d : data) {
            d = gen() & 0xFF;
        }
    }

    if (t == throw_on_connect) {
        throw std::runtime_error("connect");
    }

    co_yield dsize;

    if (t == throw_before_chunks) {
        throw std::runtime_error("before chunks");
    }

    size_t ptr = 0;
    while (ptr < dsize) {
        auto& buf = co_await ahttp::sync_generator::chunk_buf_t{};
        if (ptr == 0) {
            if (t == throw_on_first_chunk) {
                throw std::runtime_error("first chunk");
            }
        }
        else if (t == throw_on_second_chunk) {
            throw std::runtime_error("second chunk");
        }

        auto n = std::min(dsize - ptr, buf.size());
        for (size_t i = 0; i < n; ++i) {
            buf[i] = data[ptr + i];
        }
        ptr += n;
        buf = buf.subspan(0, n);
    }

    if (t == throw_on_disconnect) {
        throw std::runtime_error("disconnect");
    }
}

bool data_coherent(std::span<const uint8_t> data) {
    std::minstd_rand gen(seed);
    for (auto d : data) {
        if (d != (gen() & 0xFF)) {
            return false;
        }
    }
    return true;
}

constexpr size_t chunk_size = 200;

void do_test(std::vector<uint8_t>& data, test_type tt) {
    auto gen = test(tt);
    CHECK(gen.size().value_or(0) == 1024);
    data.reserve(gen.size().value_or(10));

    while (!gen.done()) {
        std::vector<uint8_t> buf(chunk_size);
        auto chunk = gen.get_next_chunk(buf);
        if (chunk.size() != chunk_size) {
            CHECK(chunk.size() == dsize % chunk_size);
        }
        data.insert(data.end(), chunk.begin(), chunk.end());
        CHECK(data_coherent(data));
    }
}

TEST_CASE("no throw") {
    std::vector<uint8_t> data;
    do_test(data, no_throw);
    CHECK(data.size() == dsize);
}

TEST_CASE("throw on connect") {
    std::vector<uint8_t> data;
    CHECK_THROWS_WITH_AS(do_test(data, throw_on_connect), "connect", std::runtime_error);
    CHECK(data.capacity() == 0);
}

TEST_CASE("throw before chunks") {
    std::vector<uint8_t> data;
    CHECK_THROWS_WITH_AS(do_test(data, throw_before_chunks), "before chunks", std::runtime_error);
    CHECK(data.capacity() == 0);
}

TEST_CASE("throw on first chunk") {
    std::vector<uint8_t> data;
    CHECK_THROWS_WITH_AS(do_test(data, throw_on_first_chunk), "first chunk", std::runtime_error);
    CHECK(data.capacity() == dsize);
    CHECK(data.size() == 0);
}

TEST_CASE("throw on second chunk") {
    std::vector<uint8_t> data;
    CHECK_THROWS_WITH_AS(do_test(data, throw_on_second_chunk), "second chunk", std::runtime_error);
    CHECK(data.capacity() == dsize);
    CHECK(data.size() == chunk_size);
}

TEST_CASE("throw on disconnect") {
    std::vector<uint8_t> data;
    CHECK_THROWS_WITH_AS(do_test(data, throw_on_disconnect), "disconnect", std::runtime_error);
    CHECK(data.capacity() == dsize);
    CHECK(data.size() == dsize - dsize % chunk_size);
}
