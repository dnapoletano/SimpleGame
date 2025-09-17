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
         SDL_WINDOW_RESIZABLE),
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
      new Renderer(_device),
      {}
   };
}

Window::~Window() {
   SDL_Quit();
}

auto Window::update(Scene& scene) -> void {
   const auto surface = _layer->nextDrawable();
   SDL_PollEvent(&_event);
   auto camera = *scene.getCamera();

   auto eye = Vector3{0.0f,0.0f,0.0f};
   float yaw = 0.0f;
   float pitch = 0.0f;
   switch (_event.type) {
      case SDL_QUIT:
         _is_running = false;
         break;
      case SDL_KEYDOWN:
         switch (_event.key.keysym.sym) {
            case SDLK_RIGHT:
            case SDLK_d:
               eye = camera.getRight();
               break;
         case SDLK_LEFT:
         case SDLK_a:
               eye = -camera.getRight();
               break;
         case SDLK_UP:
         case SDLK_w:
               eye = camera.getDirection();
               break;
         case SDLK_DOWN:
         case SDLK_s:
               eye = -camera.getDirection();
               break;
         case SDLK_ESCAPE:
               _is_running = false;
               break;
         default:
               break;
         }
         break;

      case SDL_MOUSEMOTION:
         yaw   = static_cast<float>(_event.motion.xrel)*0.02f;
         pitch = static_cast<float>(_event.motion.yrel)*0.02f;
         camera.adjust_yaw(yaw);
         camera.adjust_pitch(-pitch);
         break;
      default:
         break;
   }

   camera.translate(eye);
   _renderer->render(camera, surface,scene);
}

auto Window::running() const -> bool {
   return _is_running;
}
}
