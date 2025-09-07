add_executable(game_tutorial
        main.cpp
        window.cpp
        exception.cpp
        error.hpp
        mesh.cpp)

target_compile_features(game_tutorial PUBLIC cxx_std_23)

#target_compile_definitions(game_tutorial PUBLIC
#        NS_PRIVATE_IMPLEMENTATION
#        CA_PRIVATE_IMPLEMENTATION
#        MTL_PRIVATE_IMPLEMENTATION
#)

target_compile_options(game_tutorial PUBLIC -Wall -Wextra -Werror -g)
target_include_directories(game_tutorial PUBLIC
        /opt/homebrew/include
        ${PROJECT_SOURCE_DIR}/metal-cpp
)

target_link_options(game_tutorial PUBLIC "SHELL:-framework Foundation -framework Metal -framework MetalKit")
target_link_directories(game_tutorial PUBLIC /opt/homebrew/lib)
target_link_libraries(game_tutorial PUBLIC SDL2)
