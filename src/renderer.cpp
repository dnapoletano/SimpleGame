#include "renderer.hpp"

namespace game {
Renderer::Renderer(MTL::Device* device)
   : _device(device){
}

auto Renderer::createDepthTexture(const std::uint32_t width, const std::uint32_t height) -> void {
   const auto depthDescriptor = AutoRelease<MTL::TextureDescriptor*>{
      MTL::TextureDescriptor::alloc()->init(),
      [](auto t) {t->release();}
   };
   depthDescriptor->setPixelFormat(MTL::PixelFormatDepth32Float);
   depthDescriptor->setSampleCount(1);
   depthDescriptor->setWidth(width);
   depthDescriptor->setHeight(height);
   depthDescriptor->setUsage(MTL::TextureUsageRenderTarget | MTL::TextureUsageShaderRead);
   _depthTexture = AutoRelease<MTL::Texture*>{
      _device->newTexture(depthDescriptor.get()),
      [](auto t) {t->release();}
   };
}

auto Renderer::render(const Camera& camera,
      CA::MetalDrawable* surface, Scene& scene) const -> void {
   const MTL::ClearColor clear_color{0.,0.,0.,1.};
   const auto commandQueue = _device->newCommandQueue();
   const auto buffer = commandQueue->commandBuffer();

   const auto renderPassDescriptor = MTL::RenderPassDescriptor::alloc()->init();
   const auto cd = renderPassDescriptor->colorAttachments()->object(0);
   cd->setTexture(surface->texture());

   cd->setLoadAction(MTL::LoadActionClear);
   renderPassDescriptor->depthAttachment()->setLoadAction(MTL::LoadActionClear);
   cd->setClearColor(clear_color);
   cd->setStoreAction(MTL::StoreActionStore);
   renderPassDescriptor->depthAttachment()->setStoreAction(MTL::StoreActionStore);
   renderPassDescriptor->depthAttachment()->setClearDepth(1.0);
   renderPassDescriptor->depthAttachment()->setTexture(_depthTexture.get());
   const auto encoder = buffer->renderCommandEncoder(renderPassDescriptor);
   scene.setCamera(const_cast<Camera*>(&camera));
   scene.render(encoder);
   scene.renderSkyBox(encoder);

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