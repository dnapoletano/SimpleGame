#include <numbers>
#include <simd/simd.h>


#include "error.hpp"
#include "matrix4.hpp"
#include "renderer.hpp"


namespace game {

Renderer::Renderer(MTL::Device* device, CA::MetalLayer* layer)
   : _device(device), _layer(layer){
}

auto Renderer::setMeshBufer(const Entity& entity) -> void {
   _mesh_buffer = _device->newBuffer(entity.getVertexData(),entity.size(),{});
}

auto Renderer::setUpPipelineState(ShaderFunctions funcs) -> void {
   const auto pipeline_descriptor = AutoRelease<MTL::RenderPipelineDescriptor *,{}>{
      MTL::RenderPipelineDescriptor::alloc()->init(),
      [](auto t) {t->release();}};

   const auto attach = pipeline_descriptor->colorAttachments()->object(0);

   pipeline_descriptor->setVertexFunction(funcs["vertexShader"]);
   pipeline_descriptor->setFragmentFunction(funcs["fragmentShader"]);
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
   const auto depth_stencil_descriptor = AutoRelease<MTL::DepthStencilDescriptor*,{}>{
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

auto Renderer::render(const Camera& camera, CA::MetalDrawable * surface) const -> void {
   const MTL::ClearColor clear_color{0.,0.,0.,1.};
   const auto commandQueue = _device->newCommandQueue();
   const auto buffer = commandQueue->commandBuffer();

   const auto renderPassDescriptor = MTL::RenderPassDescriptor::alloc()->init();
   const auto cd = renderPassDescriptor->colorAttachments()->object(0);
   cd->setTexture(surface->texture());

   cd->setLoadAction(MTL::LoadActionClear);
   cd->setClearColor(clear_color);
   cd->setStoreAction(MTL::StoreActionStore);

   const auto encoder = buffer->renderCommandEncoder(renderPassDescriptor);
   encoder->setRenderPipelineState(_rps.get());

   encoder->setVertexBuffer(_mesh_buffer,0,0);

   const Matrix4 rot = {};
   

   encoder->setFrontFacingWinding(MTL::WindingCounterClockwise);
   encoder->setCullMode(MTL::CullModeBack);
   encoder->setDepthStencilState(_dss.get());
   encoder->setVertexBytes(&rot.data(),sizeof(rot.data()),1);
   encoder->setVertexBytes(camera.getData(),camera.size(),2);

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
}

}