# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
ac_dep(furi)
ac_dep(boost)
ac_dep(openssl)

icm_add_lib(ahttp AHTTP
    code/ahttp/export.h
    code/ahttp/ahttp.hpp
    code/ahttp/ahttp.cpp
)
target_include_directories(ahttp INTERFACE code)
target_link_libraries(ahttp
    PUBLIC
        splat::splat
        itlib::itlib
        ac::astl
    PRIVATE
        furi::furi
        Boost::beast
)
add_library(ac::http ALIAS ahttp)

if(TARGET OpenSSL::SSL)
    target_compile_definitions(ahttp PUBLIC AHTTP_SSL=1)
    target_link_libraries(ahttp PRIVATE OpenSSL::SSL)
endif()

ac_add_test_subdir()
