// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <acord/server/App.hpp>
#include <ac/jalog/Instance.hpp>
#include <ac/jalog/sinks/DefaultSink.hpp>
#include <string.h>

using namespace acord::server;

int main(int argc, char** argv) {
    ac::jalog::Instance jl;
    jl.setup().async().add<ac::jalog::sinks::DefaultSink>();

    acord::server::AppParams params;
    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "--port") == 0) {
            if (i + 1 < argc) {
                params.wsPort = atoi(argv[i + 1]);
                i++;
            }
        } else if (strcmp(argv[i], "--all") == 0) {
            params.serveLocalhostOnly = false;
        }
    }

    App app(params);
    app.run();

    // should never get to here
    return 0;
}
