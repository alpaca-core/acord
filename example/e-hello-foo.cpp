#include <acord/client/Connection.hpp>

#include <ac/frameio/local/LocalChannelUtil.hpp>
#include <ac/frameio/local/LocalBufferedChannel.hpp>
#include <ac/frameio/local/BlockingIo.hpp>
#include <ac/schema/BlockingIoHelper.hpp>

#include <ac/jalog/Instance.hpp>
#include <ac/jalog/sinks/DefaultSink.hpp>

#include "FooSchema.hpp"

#include <iostream>

namespace schema = ac::schema::foo;

int main() try {
    ac::jalog::Instance jl;
    jl.setup().add<ac::jalog::sinks::DefaultSink>();

    auto cl2r = ac::frameio::LocalBufferedChannel_create(10);
    auto cr2l = ac::frameio::LocalBufferedChannel_create(10);

    auto [local, remote] = LocalChannel_getEndpoints(cl2r, cr2l);

    acord::client::Connection con;
    con.initiate("localhost", 7654, std::move(remote));

    ac::schema::BlockingIoHelper io(ac::frameio::BlockingIo(std::move(local)));

    io.expectState<schema::StateInitial>();
    io.call<schema::StateInitial::OpLoadModel>({});

    io.expectState<schema::StateModelLoaded>();
    io.call<schema::StateModelLoaded::OpCreateInstance>({.cutoff = 2});

    io.expectState<schema::StateInstance>();
    auto result = io.call<schema::StateInstance::OpRun>({
        .input = std::vector<std::string>{"a", "b", "c"}
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
