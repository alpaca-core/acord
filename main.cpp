#include <ac/local/Lib.hpp>
#include <ac/frameio/local/LocalIoRunner.hpp>
#include <ac/frameio/local/BlockingIo.hpp>

#include <ac/jalog/Instance.hpp>
#include <ac/jalog/sinks/DefaultSink.hpp>

#include <iostream>

#ifdef HAVE_ACLP_OUT_DIR
#include "aclp-out-dir.h"
#endif

int main() {
    ac::jalog::Instance jl;
    jl.setup().add<ac::jalog::sinks::DefaultSink>();

#ifdef HAVE_ACLP_OUT_DIR
    ac::local::Lib::addPluginDir(ACLP_OUT_DIR);
#endif

    ac::local::Lib::loadAllPlugins();

    ac::frameio::LocalIoRunner io;
    auto fooHandler = ac::local::Lib::createSessionHandler("foo");
    auto foo = io.connectBlocking(std::move(fooHandler));

    foo.poll(); // state info from plugin (dummy)
    foo.push({ "load_model", {} });
    foo.poll(); // load_model response
    foo.poll(); // state info from plugin (model loaded)
    foo.push({ "create_instance", {} });
    foo.poll(); // creat_instance response
    foo.poll(); // state info from plugin (instance created)
    foo.push({ "run", {{"input", {"The", "song", "goes:"}}, {"splice", false}} });

    auto result = foo.poll(); // run response
    std::cout << result.frame.data << "\n";
    return 0;
}