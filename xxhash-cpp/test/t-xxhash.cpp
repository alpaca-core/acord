// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <xxhash/hash.hpp>
#include <doctest/doctest.h>

template <typename Hasher>
void test_hello_world(typename Hasher::hash_t expected) {
    const auto hhello = Hasher::hash("hello world");
    CHECK(hhello == expected);

    Hasher hasher;

    auto hash_piecewise = [&]() {
        hasher.update("hello");
        hasher.update(' ');
        hasher.update("world");
    };

    hash_piecewise();
    CHECK(hhello == hasher.digest());

    hasher.reset();
    hash_piecewise();
    CHECK(hhello == hasher.digest());

    auto hhello8765 = Hasher::hash("hello world", 8765);
    hasher.reset(8765);
    hash_piecewise();
    CHECK(hhello8765 == hasher.digest());

    hasher.reset();
    hasher.update("helzzz", 3);
    hasher.update("lo");
    hasher.update(' ');
    hasher.update("worlz", 4);
    hasher.update('d');
    CHECK(hhello == hasher.digest());
}

TEST_CASE("xxh64") {
    test_hello_world<xxhash::h64>(5020219685658847592ull);
}

TEST_CASE("xxh128") {
    test_hello_world<xxhash::h128>({12221510953804871367ull, 16108542339571992832ull});
}

template <typename Hasher>
void test_trivial_types() {
    Hasher hasher;
    typename Hasher::hash_t expected;

    auto run_test_case = [&](auto v, int seed = 0) {
        hasher.reset(seed);
        hasher.update(v);
        expected = Hasher::hash(v, seed);
        CHECK(expected == hasher.digest());
    };

    auto run_test_cases = [&](int seed = 0) {
        run_test_case('a', seed);
        run_test_case(true, seed);
        run_test_case(false, seed);
        run_test_case((short)15123, seed);
        run_test_case((unsigned short)3953, seed);
        run_test_case(344780572, seed);
        run_test_case(3440884442u, seed);
        run_test_case(2955292745ll, seed);
        run_test_case(4284835369ull, seed);
        run_test_case(180458.359f, seed);
        run_test_case(2564150975.125, seed);
        struct { int A; unsigned long long B; } testStruct = {1, 231231232ull};
        run_test_case(&testStruct, seed);
    };

    run_test_cases();
    run_test_cases(3173872963u);
    run_test_cases(-123125);
}

TEST_CASE("xxh64 trivial types") {
    test_trivial_types<xxhash::h64>();
}

TEST_CASE("xxh128 trivial types") {
    test_trivial_types<xxhash::h128>();
}
