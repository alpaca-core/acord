# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
macro(asset_mgr_test test)
    add_doctest_lib_test(${test} ac-asset-mgr t-${test}.cpp
        LIBRARIES
            ac-dev::test-assets
            ac-dev::test-util
    )
endmacro()

asset_mgr_test(FsUtil)
asset_mgr_test(SourceLocalDir)
asset_mgr_test(SourceLocalFiles)
asset_mgr_test(SourceHttp)
asset_mgr_test(Manager)
