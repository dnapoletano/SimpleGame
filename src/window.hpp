#ifndef GAME_TUTORIAL_WINDOW_HPP
#define GAME_TUTORIAL_WINDOW_HPP


#include <Foundation/Foundation.hpp>
#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>

#include <SDL2/SDL.h>
#include <SDL2/SDL_metal.h>

#include "auto_release.hpp"

namespace game {
   typedef std::pair<MTL::Function *,MTL::Function *> ShaderFunctions;
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
   auto setUpPipelineState(const ShaderFunctions &funcs) -> void;

   auto setMeshBufer(MTL::Buffer* buff) -> void  {_mesh_buffer = buff;}


private:
   AutoRelease<::SDL_Window*,{}>                  _window{};
   AutoRelease<::SDL_MetalView,{}>                _view{};
   AutoRelease<MTL::RenderPipelineState*,{}>      _rps{};
   SDL_Event       _event{};
   bool            _is_running{false};
   MTL::Device*    _device{nullptr};
   CA::MetalLayer* _layer{nullptr};
   MTL::Buffer* _mesh_buffer{nullptr};
};
}

#endif //GAME_TUTORIAL_WINDOW_HPP