find_package(Catch2 3 REQUIRED)

add_executable(kflat_test
)

target_link_libraries(kflat_test PRIVATE
        kflat_lib
        Catch2::Catch2WithMain
)

include(Catch)
catch_discover_tests(kflat_test)
