#include <print>
#include <iostream>
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
   pipeline_descriptor->setSampleCount(4);
   pipeline_descriptor->setDepthAttachmentPixelFormat(MTL::PixelFormatDepth32Float);
   attach->setPixelFormat(_layer->pixelFormat());
   NS::Error * error = nullptr;
   _rps = {
      _device->newRenderPipelineState(pipeline_descriptor.get(),&error),
      [](auto t){t->release();}
   };

   game::ensure(_rps.get() != nullptr,
      std::format("non pipeline descriptor ->\n{}",
               (error!=nullptr)?error->description()->utf8String():"")
      );
   auto depth_stencil_descriptor = AutoRelease<MTL::DepthStencilDescriptor*,{}>{
      MTL::DepthStencilDescriptor::alloc()->init(),
      [](auto t) {t->release();}
   };
   depth_stencil_descriptor->setDepthCompareFunction(MTL::CompareFunctionLess);
   depth_stencil_descriptor->setDepthWriteEnabled(true);
   _dss = {
      _device->newDepthStencilState(depth_stencil_descriptor.get()),
      [](auto t) {t->release();}
   };
}

auto Window::update() -> void {
   SDL_PollEvent(&_event);

   const auto surface = _layer->nextDrawable();
   const MTL::ClearColor clear_color{0.,0.,0.,1.};//152.0/255.0, 23.0/255.0, 42.0/255.0, 1.};
   const auto commandQueue = _device->newCommandQueue();
   const auto buffer = commandQueue->commandBuffer();
   // const auto renderPassDescriptor = AutoRelease<MTL::RenderPassDescriptor*,{}>{
   //    MTL::RenderPassDescriptor::alloc()->init(),
   //    [](auto t) {t->release();}
   // };
   const auto renderPassDescriptor = MTL::RenderPassDescriptor::alloc()->init();
   const auto cd = renderPassDescriptor->colorAttachments()->object(0);
   cd->setTexture(surface->texture());

   cd->setLoadAction(MTL::LoadActionClear);
   cd->setClearColor(clear_color);
   cd->setStoreAction(MTL::StoreActionStore);

   const auto encoder = buffer->renderCommandEncoder(renderPassDescriptor);
   encoder->setRenderPipelineState(_rps.get());
   encoder->setVertexBuffer(_mesh_buffer,0,0);

   static auto t = 0.0f;
   static auto x = 0.0f;
   static auto z = 0.0f;
   x = 2.f*simd::sin(t);
   z = 2.f*simd::cos(t);
   t+=0.01;

   const auto rot = game::Matrix4::lookAt(
      {x, z, z},
      {0.0f, 0.0f, 0.0f},
      {0.0f, 1.0f, 0.0f}
      );

   const auto proj = Matrix4::perspective(
      std::numbers::pi_v<float>/4.0f,
      static_cast<float>(surface->layer()->drawableSize().width),
      static_cast<float>(surface->layer()->drawableSize().height),
      0.1f,
      100.0f
      );
   //encoder->setTriangleFillMode(MTL::TriangleFillModeLines);
   encoder->setFrontFacingWinding(MTL::WindingCounterClockwise);
   encoder->setCullMode(MTL::CullModeBack);
   encoder->setDepthStencilState(_dss.get());
   encoder->setVertexBytes(&rot.data(),sizeof(rot.data()),1);
   encoder->setVertexBytes(&proj.data(),sizeof(proj.data()),2);
   constexpr auto typeTriangle = MTL::PrimitiveTypeTriangle;
   constexpr NS::UInteger vertexStart = 0;
   constexpr NS::UInteger vertexCount = 36;
   encoder->drawPrimitives(typeTriangle,vertexStart,vertexCount);
   encoder->endEncoding();
   buffer->presentDrawable(surface);
   buffer->commit();
   buffer->waitUntilCompleted();

   cd->release();
   encoder->release();
   buffer->release();
   surface->release();
   commandQueue->release();

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
