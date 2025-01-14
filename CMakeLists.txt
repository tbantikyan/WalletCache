# Minimum CMake version
cmake_minimum_required(VERSION 3.31)

# Project name and settings
project(WalletCache VERSION 1.0 LANGUAGES CXX)

# Specify C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

# Automatically gather all .cpp files in the src directory
file(GLOB SOURCES ${CMAKE_SOURCE_DIR}/src/*.cpp)

# Add the executable
add_executable(WalletCache main.cpp ${SOURCES})

# Find PkgConfig (for checking modules)
find_package(PkgConfig REQUIRED)

pkg_check_modules(LIBSODIUM REQUIRED libsodium)

# Include directories
target_include_directories(WalletCache PUBLIC ${CMAKE_SOURCE_DIR}/include ${LIBSODIUM_INCLUDE_DIRS})

# Link directories
target_link_directories(WalletCache PUBLIC ${LIBSODIUM_LIBRARY_DIRS})

# Link libraries
target_link_libraries(WalletCache PUBLIC ${LIBSODIUM_LIBRARIES})

# Compile options
target_compile_options(WalletCache PUBLIC ${LIBSODIUM_CFLAGS_OTHER})

# Subdirectories
add_subdirectory(tests)
