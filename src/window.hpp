#ifndef GAME_TUTORIAL_WINDOW_HPP
#define GAME_TUTORIAL_WINDOW_HPP

#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>

#include <SDL2/SDL.h>
#include <SDL2/SDL_metal.h>

#include "auto_release.hpp"
#include "renderer.hpp"

namespace game {

class Window {
public:
   Window(std::uint32_t width, std::uint32_t height);
   ~Window();

   Window(const Window&) = delete;
   auto operator=(const Window&) -> Window& = delete;

   Window(Window&&) = default;
   auto operator=(Window&&) -> Window& = default;

   [[nodiscard]] auto running() const -> bool ;
   auto update() -> void;

   [[nodiscard]] auto getDevice() const -> MTL::Device* {return _device;}
   [[nodiscard]] auto getRenderer() const -> game::Renderer* {return _renderer.get();}


private:
   AutoRelease<::SDL_Window*,{}>                  _window{};
   AutoRelease<::SDL_MetalView,{}>                _view{};

   AutoRelease<game::Renderer *, {}> _renderer;

   SDL_Event       _event{};
   bool            _is_running{false};

   MTL::Device*    _device{nullptr};
   CA::MetalLayer* _layer{nullptr};

};
}

#endif //GAME_TUTORIAL_WINDOW_HPP