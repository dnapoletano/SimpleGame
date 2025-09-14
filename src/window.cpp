#include <print>

#include <numbers>
#include "error.hpp"
#include "window.hpp"

#include "mesh.hpp"
#include "matrix4.hpp"

namespace game {

Window::Window(const std::uint32_t width, const std::uint32_t height){
   SDL_Init(SDL_INIT_EVERYTHING);
   _window = {
      ::SDL_CreateWindow("Game",
         SDL_WINDOWPOS_CENTERED,
         SDL_WINDOWPOS_CENTERED,
         static_cast<int>(width),
         static_cast<int>(height),
         0),
      ::SDL_DestroyWindow
   };
   game::ensure(_window.get()!=nullptr,
      "Couldn't create window");
   _view = {
      ::SDL_Metal_CreateView(_window.get()),
      ::SDL_Metal_DestroyView
   };
   game::ensure(_view.get()!=nullptr,
      "Couldn't get metal view");

   _device = MTL::CreateSystemDefaultDevice();
   game::ensure(_device!=nullptr,
      "Couldn't get default gpu");

   _layer = static_cast<CA::MetalLayer *>(::SDL_Metal_GetLayer(_view.get()));
   game::ensure(_layer!=nullptr,
      "Couldn't get metal layer");
   _layer->drawableSize() = CGSizeMake(width, height);
   _layer->setDevice(_device);
   _is_running = true;

   _renderer = {
      new Renderer(_device,_layer),
      {}
   };
}

Window::~Window() {
   SDL_Quit();
}

auto Window::update() -> void {
   const auto surface = _layer->nextDrawable();
   SDL_PollEvent(&_event);
   static auto t = 0.0f;
   static auto x = 0.0f;
   static auto z = 0.0f;
   x = 2.f*simd::sin(t);
   z = 2.f*simd::cos(t);
   t+=0.01;
   const Camera camera{std::numbers::pi_v<float> / 4.0f,
                       static_cast<float>(surface->layer()->drawableSize().width),
                       static_cast<float>(surface->layer()->drawableSize().height),
                       0.1f,
                       100.0f,
                       {x, z, z},
                       {0.0f, 0.0f, 0.0f},
                       {0.0f, 1.0f, 0.0f}};

   _renderer->render(camera, surface);

   switch (_event.type) {
      case SDL_QUIT:
         _is_running = false;
         break;
      default:
         break;
   }
}

auto Window::running() const -> bool {
   return _is_running;
}
}
