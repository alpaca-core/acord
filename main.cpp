#include <ac/local/Model.hpp>
#include <ac/local/Instance.hpp>
#include <ac/local/Lib.hpp>
#include <ac/local/ModelLoaderRegistry.hpp>

#include <ac/jalog/Instance.hpp>
#include <ac/jalog/sinks/DefaultSink.hpp>

#include <iostream>

#ifdef HAVE_ACLP_OUT_DIR
#include "aclp-out-dir.h"
#endif

int main() try {
    ac::jalog::Instance jl;
    jl.setup().add<ac::jalog::sinks::DefaultSink>();

#ifdef HAVE_ACLP_OUT_DIR
    ac::local::Lib::addPluginDir(ACLP_OUT_DIR);
#endif

    ac::local::Lib::loadAllPlugins();

    auto model = ac::local::Lib::modelLoaderRegistry().createModel({
        .inferenceType = "foo",
        .name = "foo-synthetic"
        }, {});

    auto instance = model->createInstance("general", {});

    auto opResult = instance->runOp("run", { {"input", {"The", "song", "goes:"}}, {"splice", false}});

    std::cout << opResult << "\n";
}
catch (std::exception& e) {
    std::cerr << "exception: " << e.what() << "\n";
    return 1;
}
