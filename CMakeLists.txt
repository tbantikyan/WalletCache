cmake_minimum_required(VERSION 3.31)
project(WalletCache VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()
set(CLIP_TESTS OFF CACHE BOOL "Disable clip tests" FORCE)

include(FetchContent)

FetchContent_Declare(
  clip
  GIT_REPOSITORY https://github.com/dacap/clip.git
  GIT_TAG        a1a5fd1
)
FetchContent_MakeAvailable(clip)

# Create a library instead of putting everything in the executable
file(GLOB SOURCES ${CMAKE_SOURCE_DIR}/src/*.cpp)

add_library(WalletCacheLib STATIC ${SOURCES})  # Core functionality as library

# Main executable
add_executable(WalletCache main.cpp)
target_link_libraries(WalletCache PRIVATE WalletCacheLib)

# Dependencies for both library and executable
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBSODIUM REQUIRED libsodium)

target_include_directories(WalletCacheLib PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${clip_SOURCE_DIR}
    ${LIBSODIUM_INCLUDE_DIRS}
)

target_link_directories(WalletCacheLib PUBLIC 
    ${LIBSODIUM_LIBRARY_DIRS}
)

target_link_libraries(WalletCacheLib PUBLIC
    clip
    ${LIBSODIUM_LIBRARIES}
)

target_compile_options(WalletCacheLib PUBLIC
    ${LIBSODIUM_CFLAGS_OTHER}
)

add_subdirectory(tests)
