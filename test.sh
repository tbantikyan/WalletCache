cmake --build build 
GTEST_COLOR=1 ctest --test-dir build --output-on-failure -j12
