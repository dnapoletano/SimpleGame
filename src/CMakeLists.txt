add_executable(game_tutorial
        main.cpp
        window.cpp
        exception.cpp)

target_compile_features(game_tutorial PUBLIC cxx_std_23)

target_compile_options(game_tutorial PUBLIC --target=x86_64-windows-msvc -Wall -g)
target_include_directories(game_tutorial PUBLIC
        /opt/homebrew/Cellar/msvc-wine/vc/tools/msvc/14.44.35207/atlmfc/include
        /opt/homebrew/Cellar/msvc-wine/vc/tools/msvc/14.44.35207/include
        /opt/homebrew/Cellar/msvc-wine/kits/10/include/10.0.26100.0/shared
        /opt/homebrew/Cellar/msvc-wine/kits/10/include/10.0.26100.0/ucrt
        /opt/homebrew/Cellar/msvc-wine/kits/10/include/10.0.26100.0/um
        /opt/homebrew/Cellar/msvc-wine/kits/10/include/10.0.26100.0/winrt
        /opt/homebrew/Cellar/msvc-wine/kits/10/include/10.0.26100.0/km
)
target_include_directories(game_tutorial PUBLIC
        ${PROJECT_SOURCE_DIR}
)
target_link_options(game_tutorial PUBLIC --target=x86_64-windows-msvc)
target_link_libraries(game_tutorial PUBLIC User32 Opengl32 Gdi32)
target_compile_definitions(game_tutorial PUBLIC NOMINMAX WIN32)