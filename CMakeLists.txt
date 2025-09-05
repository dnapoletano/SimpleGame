cmake_minimum_required(VERSION 4.0)
#set(CMAKE_C_COMPILER  /opt/homebrew/Cellar/gcc/15.1.0/bin/gcc-15)
#set(CMAKE_CXX_COMPILER   /opt/homebrew/Cellar/gcc/15.1.0/bin/g++-15)

project(game_tutorial
        VERSION "0.0.1"
        DESCRIPTION "test game tutorial"
        LANGUAGES C CXX)

add_subdirectory(src)
