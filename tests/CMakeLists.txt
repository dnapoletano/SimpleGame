FetchContent_MakeAvailable(googletest)
include(GoogleTest)
mark_as_advanced(BUILD_GMOCK BUILD_GTEST
        gtest_hide_internal_symbols
)
add_executable(unit_tests vector3_test.cpp
        matrix_test.cpp)
target_compile_features(unit_tests PUBLIC cxx_std_23)

target_compile_options(unit_tests PUBLIC -Wall -Wextra -Werror -g)
target_include_directories(unit_tests PUBLIC ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(unit_tests gmock_main)
gtest_discover_tests(unit_tests DISCOVERY_MODE PRE_TEST)