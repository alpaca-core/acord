#include <acord/client/IoCtx.hpp>
#include <acord/schema/Acord.hpp>

#include <ac/frameio/local/LocalChannelUtil.hpp>
#include <ac/frameio/local/LocalBufferedChannel.hpp>
#include <ac/frameio/local/BlockingIo.hpp>
#include <ac/schema/BlockingIoHelper.hpp>

#include <ac/jalog/Instance.hpp>
#include <ac/jalog/sinks/DefaultSink.hpp>

#include "FooSchema.hpp"

#include <iostream>

#define SAME_PROCESS_SERVER 1
#if SAME_PROCESS_SERVER
#include <acord/server/AppRunner.hpp>
#endif

namespace acrd = ac::schema::acord;
namespace foo = ac::schema::foo;

int main() try {
    ac::jalog::Instance jl;
    jl.setup().add<ac::jalog::sinks::DefaultSink>();

#if SAME_PROCESS_SERVER
    acord::server::AppRunner appRunner;
#endif

    auto [local, remote] = LocalChannel_getEndpoints(
        ac::frameio::LocalBufferedChannel_create(10),
        ac::frameio::LocalBufferedChannel_create(10)
    );

    acord::client::IoCtx acordIo;
    acordIo.connect(std::move(remote));

    ac::schema::BlockingIoHelper io(ac::frameio::BlockingIo(std::move(local)));

    io.expectState<acrd::State>();
    auto files = io.call<acrd::State::OpMakeAssetsAvailable>(
        std::vector<std::string>{"https://raw.githubusercontent.com/alpaca-core/test-data-foo/900479ceedddc6e3867c467bbedb7a5310414041/foo-large.txt"}
    );

    io.call<acrd::State::OpLoadProvider>({.name = "foo"});

    io.expectState<foo::StateInitial>();
    io.call<foo::StateInitial::OpLoadModel>({
        .filePath = std::move(files.front())
    });

    io.expectState<foo::StateModelLoaded>();
    io.call<foo::StateModelLoaded::OpCreateInstance>({});

    io.expectState<foo::StateInstance>();
    auto result = io.call<foo::StateInstance::OpRun>({
        .input = std::vector<std::string>{"JFK", "said", ":"},
        .splice = false
    });
    std::cout << result.result.value() << std::endl;

    return 0;
}
catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
}
catch (...) {
    std::cerr << "Unknown error" << std::endl;
    return 1;
}
