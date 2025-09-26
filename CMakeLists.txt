cmake_minimum_required(VERSION 4.0)
#set(CMAKE_C_COMPILER  /opt/homebrew/Cellar/llvm/20.1.8/bin/clang)
#set(CMAKE_CXX_COMPILER   /opt/homebrew/Cellar/llvm/20.1.8/bin/clang++)

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

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_SAMPLES OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ZLIB OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_FBX_IMPORTER ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_OBJ_IMPORTER ON CACHE BOOL "" FORCE)
set(ASSIMP_NO_EXPORT ON CACHE BOOL "" FORCE)

FetchContent_Declare(
        assimp
        GIT_REPOSITORY git@github.com:assimp/assimp.git
        GIT_TAG v6.0.2
)

FetchContent_GetProperties(googletest)
FetchContent_GetProperties(stb)
FetchContent_GetProperties(assimp)

add_subdirectory(src)

enable_testing()
include(CTest)
add_subdirectory(tests)