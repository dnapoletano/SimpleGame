#ifndef GAME_TUTORIAL_WINDOW_HPP
#define GAME_TUTORIAL_WINDOW_HPP

#include <cstdint>
#include <Windows.h>

#include "auto_release.hpp"

namespace game {
class Window {
public:
   Window(std::uint32_t width, std::uint32_t height);

   Window(const Window&) = delete;
   auto operator=(const Window&) -> Window& = delete;

   Window(Window&&) = default;
   auto operator=(Window&&) -> Window& = default;

   [[nodiscard]] auto running() const -> bool ;
   auto swap() const -> void;
private:
   ::WNDCLASSA _wc;
   AutoRelease<::HWND,{}> _win;
   AutoRelease<::HDC,{}>  _dc;

   static auto _resolve_wgl_functions(::HINSTANCE instance) -> void;
   template<class T>
   static auto _resolve_gl_functions(T &function, const std::string& name) -> void;
   static auto _init_opengl(HDC dc) -> void;
   static auto _resolve_global_opengl_functions() -> void;
};
}

#endif //GAME_TUTORIAL_WINDOW_HPP