// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "FsUtil.hpp"
#include <astl/throw_stdex.hpp>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <sys/stat.h>
#if defined(_WIN32)
#   define stat _stat
#endif

namespace acord::asset::fs {

using ac::throw_ex;

namespace sfs = std::filesystem;

std::string getTempDir() {
    auto ret = sfs::temp_directory_path().string();
    if (ret.empty()) {
        throw_ex{} << "Failed to get temp directory";
    }
    if (ret.back() == '/' || ret.back() == '\\') {
        ret.pop_back();
    }
    return ret;
}

std::string getFileName(std::string_view path) {
    return sfs::path(path).filename().string();
}

constexpr const char* Home_Var =
#if defined(_WIN32)
    "USERPROFILE";
#else
    // just use $HOME for everything else
    "HOME";
#endif
;

void expandPathInPlace(std::string& path) {
    if (path.starts_with('~')) {
        auto home = std::getenv(Home_Var);

        if (!home) {
            throw_ex{} << "Environment variable " << Home_Var << " not set";
        }

        path.replace(0, 1, home);
        return;
    }

    if (!path.starts_with('$')) return;

    auto end = path.find_first_of('/');
    if (end == std::string::npos) {
        end = path.size();
    }

    auto var = path.substr(1, end - 1);
    auto val = std::getenv(var.data());
    if (!val) {
        throw_ex{} << "Environment variable " << var << " not set";
    }

    path.replace(0, end, val);
}


std::string expandPath(std::string_view path) {
    auto ret = std::string(path);
    expandPathInPlace(ret);
    return ret;
}

BasicStat basicStat(const std::string& path) noexcept {
    struct stat st;
    if (stat(path.data(), &st) != 0) {
        return {BasicStat::NotExist, 0};
    }
    if (st.st_mode & S_IFREG) {
        return {BasicStat::File, static_cast<uint64_t>(st.st_size)};
    }
    if (st.st_mode & S_IFDIR) {
        return {BasicStat::Directory, 0};
    }
    return {BasicStat::Other, 0};
}

bool mkdir_p(std::string_view path) {
    sfs::path fspath(path);
    return sfs::create_directories(fspath);
}

bool rm_r(std::string_view path, bool f) {
    sfs::path fspath(path);
    if (f) {
        return !!sfs::remove_all(fspath);
    }
    return sfs::remove(fspath);
}

void touch(const std::string& path, bool mkdir) {
    if (mkdir) {
        auto fspath = sfs::path(path).parent_path();
        if (!fspath.empty()) {
            sfs::create_directories(fspath);
        }
    }
    auto f = fopen(path.c_str(), "a+");
    if (!f) {
        throw_ex{} << "Failed to touch file: " << path;
    }
    fclose(f);
}

void mv(std::string_view src, std::string_view tgt) {
    sfs::rename(
        sfs::path(src),
        sfs::path(tgt)
    );
}

} // namespace ac::asset::fs
