// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//

// inl file: no include guards, no nothing

TEST_CASE("just get") {
    auto gen = ahttp::get_sync(SCHEME "://httpbin.org/bytes/128?seed=42");
    auto size = gen.size();
    CHECK(size.value_or(0) == 128);
    std::vector<uint8_t> data(128);
    auto chunk = gen.get_next_chunk(data);
    CHECK(chunk.size() == 128);
    CHECK(gen.done());
    CHECK(is_expectected(chunk));
    CHECK(is_expectected(data));
}

TEST_CASE("chunked get") {
    auto gen = ahttp::get_sync(SCHEME "://httpbin.org/bytes/512?seed=42");
    auto size = gen.size();
    CHECK(size.value_or(0) == 512);

    std::vector<uint8_t> data;
    while (!gen.done()) {
        std::vector<uint8_t> buf(150 + rand() % 100);
        auto chunk = gen.get_next_chunk(buf);
        data.insert(data.end(), chunk.begin(), chunk.end());
    }
    CHECK(data.size() == 512);
    CHECK(is_expectected(data));
}

TEST_CASE("redirect once") {
    auto gen = ahttp::get_sync(SCHEME "://httpbin.org/redirect-to?url=" SCHEME "%3A%2F%2Fhttpbin.org%2Fbytes%2F64%3Fseed%3D42");
    auto size = gen.size();
    CHECK(size.value_or(0) == 64);
    std::vector<uint8_t> data(64);
    auto chunk = gen.get_next_chunk(data);
    CHECK(chunk.size() == 64);
    CHECK(gen.done());
    CHECK(is_expectected(chunk));
}

TEST_CASE("redirect more") {
    auto gen = ahttp::get_sync(SCHEME "://httpbin.org/redirect/5");
    auto size = gen.size();
    CHECK(!!size);
    std::string data;
    data.resize(*gen.size());
    auto chunk = gen.get_next_chunk(std::span(reinterpret_cast<uint8_t*>(data.data()), data.size()));
    CHECK(chunk.size() == data.size());
    CHECK(gen.done());

    // data should be a json with our request
    // {
    //   "args": {},
    //   "headers": {
    //      "Accept": "*/*",
    //      "Host": "httpbin.org",
    //      "User-Agent": "ac-file-download/1.0.0",
    //      "X-Amzn-Trace-Id": "Root=<guid>"
    //   },
    //   "origin": "<ip address>",
    //   "url": "http://httpbin.org/get"
    // }
    // so, check for some key fields
    // C++23: use contains instead of find
    CHECK(data.find(R"json("Host": "httpbin.org")json") != std::string::npos);
    CHECK(data.find(R"json("User-Agent": "ac-file-download/)json") != std::string::npos);
    CHECK(data.find(R"json("url": ")json" SCHEME "://httpbin.org/get") != std::string::npos);
}

TEST_CASE("stream chunked") {
    auto gen = ahttp::get_sync(SCHEME "://httpbin.org/stream-bytes/128?seed=42");
    auto size = gen.size();
    CHECK_FALSE(size); // size should not be available on stream

    std::vector<uint8_t> data;
    while (!gen.done()) {
        std::vector<uint8_t> buf(33);
        auto chunk = gen.get_next_chunk(buf);
        data.insert(data.end(), chunk.begin(), chunk.end());
    }
    CHECK(data.size() == 128);
    CHECK(is_expectected(data));
}
