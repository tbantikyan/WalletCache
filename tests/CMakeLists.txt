include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.15.2
)
FetchContent_MakeAvailable(googletest)

include(GoogleTest)

function(config_test test_name test_source)
    add_executable(${test_name} ${test_source})
    
    # Link to the library instead of executable
    target_link_libraries(${test_name} PRIVATE
        WalletCacheLib  # Our core library
        GTest::gtest_main
        GTest::gmock
    )
    
    target_include_directories(${test_name} PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/mocks
    )
    
    gtest_discover_tests(${test_name})
endfunction()

# Create test - no need to specify implementation files
config_test(creditcard_test creditcard_test.cpp)
config_test(fstreamfileio_test fstreamfileio_test.cpp)
config_test(store_test store_test.cpp)
config_test(sodiumcrypto_test sodiumcrypto_test.cpp)
config_test(ui_test ui_test.cpp)
config_test(verification_test verification_test.cpp)
