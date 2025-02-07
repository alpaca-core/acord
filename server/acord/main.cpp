// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <acord/server/App.hpp>
#include <ac/jalog/Instance.hpp>
#include <ac/jalog/sinks/DefaultSink.hpp>

using namespace acord::server;

int main() {
    ac::jalog::Instance jl;
    jl.setup().add<ac::jalog::sinks::DefaultSink>();

    App app;
    app.run();

    // should never get to here
    return 0;
}
