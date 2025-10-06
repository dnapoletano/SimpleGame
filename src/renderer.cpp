#include "renderer.hpp"

namespace game {
Renderer::Renderer(MTL::Device* device)
   : _device(device){
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
   cd->setClearColor(clear_color);
   cd->setStoreAction(MTL::StoreActionStore);
   /// TODO: replace these
   renderPassDescriptor->depthAttachment()->setLoadAction(MTL::LoadActionClear);
   renderPassDescriptor->depthAttachment()->setStoreAction(MTL::StoreActionDontCare);
   renderPassDescriptor->depthAttachment()->setClearDepth(1.0);

   const auto encoder = buffer->renderCommandEncoder(renderPassDescriptor);
   scene.setCamera(const_cast<Camera*>(&camera));
   scene.render(encoder);

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