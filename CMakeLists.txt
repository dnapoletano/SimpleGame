cmake_minimum_required(VERSION 4.0)
set(CMAKE_SYSTEM_NAME Generic)
# set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_C_COMPILER /opt/homebrew/Cellar/llvm/20.1.8/bin/clang)
set(CMAKE_CXX_COMPILER  /opt/homebrew/Cellar/llvm/20.1.8/bin/clang)
set(CMAKE_LINKER_TYPE lld)
set(CMAKE_C_USING_LINKER_lld "-fuse-ld=lld")
set(CMAKE_CXX_USING_LINKER_lld "-fuse-ld=lld")
set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "Embedded")
project(game_tutorial
        VERSION "0.0.1"
        DESCRIPTION "test game tutorial"
        LANGUAGES C CXX)

add_subdirectory(src)
