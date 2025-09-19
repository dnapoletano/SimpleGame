cmake_minimum_required(VERSION 4.0)
#set(CMAKE_C_COMPILER  /opt/homebrew/Cellar/gcc/15.1.0/bin/gcc-15)
#set(CMAKE_CXX_COMPILER   /opt/homebrew/Cellar/gcc/15.1.0/bin/g++-15)

project(game_tutorial
        VERSION "0.0.1"
        DESCRIPTION "test game tutorial"
        LANGUAGES C CXX)

include(FetchContent)

FetchContent_Declare( googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.15.2)

FetchContent_Declare(
        stb
        GIT_REPOSITORY https://github.com/nothings/stb.git
        GIT_TAG fede005abaf93d9d7f3a679d1999b2db341b360f
)

FetchContent_GetProperties(googletest)
FetchContent_GetProperties(stb)

add_subdirectory(src)

enable_testing()
include(CTest)
add_subdirectory(tests)