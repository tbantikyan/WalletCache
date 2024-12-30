# Minimum CMake version
cmake_minimum_required(VERSION 3.31)

# Project name and settings
project(WalletCache VERSION 1.0 LANGUAGES CXX)

# Specify C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Automatically gather all .cpp files in the src directory
file(GLOB SOURCES ${CMAKE_SOURCE_DIR}/src/*.cpp)

# Add the executable
add_executable(WalletCache main.cpp ${SOURCES})
