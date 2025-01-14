include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        v1.15.2
)

FetchContent_MakeAvailable(googletest)
add_library(GTest::GTest INTERFACE IMPORTED)
target_link_libraries(GTest::GTest INTERFACE gtest_main)

set(BINARY ${CMAKE_PROJECT_NAME}_test)

include(GoogleTest)
function(config_test test_name test_source source_name)
    add_executable(${test_name} ${test_source})
    target_include_directories(${test_name} PUBLIC ${CMAKE_SOURCE_DIR}/include)
    target_link_libraries(${test_name} PRIVATE GTest::gtest_main)
    target_sources(${test_name} PRIVATE ${CMAKE_SOURCE_DIR}/src/${source_name})
    gtest_discover_tests(${test_name})
endfunction()

config_test(utils_test utils_test.cpp utils.cpp)
config_test(verification_test verification_test.cpp verification.cpp)
