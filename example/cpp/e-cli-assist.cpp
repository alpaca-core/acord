// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <acord/client/IoCtx.hpp>
#include <acord/schema/Acord.hpp>

#include <ac/frameio/local/BufferedChannelStream.hpp>
#include <ac/frameio/local/BufferedChannel.hpp>
#include <ac/frameio/BlockingIo.hpp>
#include <ac/frameio/FrameWithStatus.hpp>
#include <ac/FrameUtil.hpp>

#include <astl/throw_stdex.hpp>

#include <iostream>
#include <string>
#include <string_view>

#if !defined(SAME_PROCESS_SERVER)
#   define SAME_PROCESS_SERVER 0
#endif

#if SAME_PROCESS_SERVER
#   include <acord/server/AppRunner.hpp>
#endif

using ac::throw_ex;

std::string getShellName() {
    // silly way to find the shell name, but it works 99% of the time
    if (const char* pshell = std::getenv("SHELL")) {
        std::string_view shell = pshell;
        if (shell.find_last_of('/') != std::string::npos) {
            return std::string(shell.substr(shell.find_last_of('/') + 1));
        }
    }

    // assume windows and use the hackiest (but only know to kinda-sorta work) way
    return system("echo %PSModulePath% | findstr /L %USERPROFILE% > NUL") == 0 ? "powershell" : "cmd";
}

struct ShellInfo {
    std::string promptName;
    std::string langName;
    char promptSym;
};

ShellInfo getShellInfo() {
    auto name = getShellName();
    if (name == "cmd") {
        return {"Windows cmd", "cmd", '>'};
    }
    else if (name == "powershell") {
        return {"Windows PowerShell", "ps", '>'};
    }
    else {
        return {name, name, '$'};
    }
}

void expectSuccess(const ac::frameio::FrameWithStatus& f) {
    if (!f.success()) {
        throw_ex{} << "Expected success, but got " << f.bits;
    }
    if (Frame_isError(f.value)) {
        throw_ex{} << "Expected success, but got error: " << Frame_getError(f.value);
    }
}

void expectStateChange(const ac::frameio::FrameWithStatus& f, std::string_view stateName) {
    if (Frame_getStateChange(f.value) != stateName) {
        throw_ex{} << "Expected state change to " << stateName << ", but got " << f.value.op;
    }
}

int main(int argc, char* argv[]) try {
    if (argc == 1) {
        std::cout << "Usage: " << argv[0] << " <query> <with> <potentially> <multiple> <words>\n";
        return 0;
    }

    auto shellInfo = getShellInfo();
    std::string prompt = "In " + shellInfo.promptName;
    for (auto i = 1; i < argc; ++i) {
        prompt += ' ';
        prompt += argv[i];
    }
    prompt += "\n```" + shellInfo.langName + "\n" + shellInfo.promptSym;

    //std::cout << prompt << std::endl;

    //std::string prompt = "In Windows cmd how to show files in current dir sorted by date?\n```cmd\n>";

#if SAME_PROCESS_SERVER
    acord::server::AppRunner appRunner;
#endif

    auto [local, remote] = ac::frameio::BufferedChannel_getEndpoints(
        ac::frameio::BufferedChannel_create(10),
        ac::frameio::BufferedChannel_create(10)
    );

    ac::frameio::BlockingIoCtx blockingCtx;
    acord::client::IoCtx acordIo;
    acordIo.connect(std::move(remote));

    ac::frameio::BlockingIo io(std::move(local), blockingCtx);

    expectStateChange(io.poll(), "acord");

    io.push({"make_assets_available", std::vector<std::string>{
    	"https://huggingface.co/Qwen/Qwen2.5-Coder-3B-Instruct-GGUF/resolve/main/qwen2.5-coder-3b-instruct-q8_0.gguf"
        //"https://huggingface.co/Qwen/Qwen2.5-7B-Instruct-GGUF/resolve/main/qwen2.5-7b-instruct-q6_k-00001-of-00002.gguf",
        //"https://huggingface.co/Qwen/Qwen2.5-7B-Instruct-GGUF/resolve/main/qwen2.5-7b-instruct-q6_k-00002-of-00002.gguf"
    }});
    auto f = io.poll(astl::timeout::after_ms(100));
    if (!f.success()) {
        std::cout << "downloading assets..." << std::endl;
        f = io.poll();
    }
    expectSuccess(f);
    auto ggufPath = f.value.data[0].get<std::string>();

    io.push({"load_provider", {{"name", "llama"}}});
    expectSuccess(io.poll());
    expectStateChange(io.poll(), "initial");

    io.push({"load-model", {{"gguf", ggufPath}}});
    expectSuccess(io.poll());
    expectStateChange(io.poll(), "model-loaded");

    io.push({"start-instance", {{"ctx_size", 2048}}});
    expectSuccess(io.poll());
    expectStateChange(io.poll(), "instance");

    io.push({"run", {
        {"prompt", prompt},
        {"antiprompts", {"```"}},
        {"max_tokens", 200},
        {"stream", false},
    }});

    f = io.poll();
    expectSuccess(f);

    auto result = f.value.data["result"].get<std::string_view>();

    if (!result.empty() && result[0] == ' ') result.remove_prefix(1);
    std::cout << result << std::endl;

    return 0;
}
catch (std::runtime_error& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
}
