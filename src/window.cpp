#include <iostream>
#include <print>

#include "error.hpp"
#include "window.hpp"

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
}

Window::~Window() {
   SDL_Quit();
}

auto Window::setUpPipelineState(const ShaderFunctions &funcs) -> void {
   const auto pipeline_descriptor = AutoRelease<MTL::RenderPipelineDescriptor *,{}>{
      MTL::RenderPipelineDescriptor::alloc()->init(),
      [](auto t) {t->release();}};

   const auto attach = pipeline_descriptor->colorAttachments()->object(0);
   pipeline_descriptor->setVertexFunction(funcs.first);
   pipeline_descriptor->setFragmentFunction(funcs.second);
   attach->setPixelFormat(_layer->pixelFormat());
   NS::Error * error = nullptr;
   _rps = {
      _device->newRenderPipelineState(pipeline_descriptor.get(),&error),
      [](auto t){t->release();}
   };

   game::ensure(_rps.get() !=nullptr,
      std::format("non pipeline descriptor ->\n{}",
               (error!=nullptr)?error->description()->utf8String():"")
      );
}

auto Window::update() -> void {
   SDL_PollEvent(&_event);

   const auto surface = _layer->nextDrawable();
   const MTL::ClearColor clear_color{0.,0.,0.,1.};//152.0/255.0, 23.0/255.0, 42.0/255.0, 1.};
   const auto commandQueue = _device->newCommandQueue();
   const auto buffer = commandQueue->commandBuffer();
   const auto renderPassDescriptor = AutoRelease<MTL::RenderPassDescriptor*,{}>{
      MTL::RenderPassDescriptor::alloc()->init(),
      [](auto t) {t->release();}
   };
   const auto cd = renderPassDescriptor->colorAttachments()->object(0);
   cd->setTexture(surface->texture());
   cd->setLoadAction(MTL::LoadActionClear);
   cd->setClearColor(clear_color);
   cd->setStoreAction(MTL::StoreActionStore);

   const auto encoder = buffer->renderCommandEncoder(renderPassDescriptor.get());
   encoder->setRenderPipelineState(_rps.get());
   constexpr auto typeTriangle = MTL::PrimitiveTypeTriangle;
   constexpr NS::UInteger vertexStart = 0;
   constexpr NS::UInteger vertexCount = 3;
   encoder->drawPrimitives(typeTriangle,vertexStart,vertexCount);
   encoder->endEncoding();
   buffer->presentDrawable(surface);
   buffer->commit();
   buffer->waitUntilCompleted();

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