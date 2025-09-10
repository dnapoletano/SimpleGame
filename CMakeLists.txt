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

FetchContent_GetProperties(googletest)

add_subdirectory(src)

enable_testing()
include(CTest)
add_subdirectory(tests)