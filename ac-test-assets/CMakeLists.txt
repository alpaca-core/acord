add_library(ac-test-assets INTERFACE)
add_library(ac-dev::test-assets ALIAS ac-test-assets)
target_link_libraries(ac-test-assets INTERFACE ac-repo-root)
target_include_directories(ac-test-assets INTERFACE .)
