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

auto Renderer::createShadowTexture(const std::uint32_t width, const std::uint32_t height) -> void {
   const auto shadowPassDescriptor = AutoRelease<MTL::TextureDescriptor*>{
      MTL::TextureDescriptor::alloc()->init(),
      [](auto t) {t->release();}
   };
   shadowPassDescriptor->setPixelFormat(MTL::PixelFormatDepth32Float);
   shadowPassDescriptor->setUsage(MTL::TextureUsageRenderTarget | MTL::TextureUsageShaderRead);
   shadowPassDescriptor->setSampleCount(1);
   shadowPassDescriptor->setWidth(width);
   shadowPassDescriptor->setHeight(height);
   shadowPassDescriptor->setStorageMode(MTL::StorageModePrivate);
   _shadowPassTexture = AutoRelease<MTL::Texture*>{
      _device->newTexture(shadowPassDescriptor.get()),
      [](auto t) {t->release();}
   };
}

auto Renderer::shadowPipelineState([[maybe_unused]]const CA::MetalLayer* const layer) -> void {
   const auto pipeline_descriptor = AutoRelease<MTL::RenderPipelineDescriptor *,{}>{
      MTL::RenderPipelineDescriptor::alloc()->init(),
      [](auto t) {t->release();}};

   const ResourceLoader rl{ROOT_DIR};
   const auto shader_path = std::filesystem::path(SHADERS_DIR) / "shadow.metal";
   const auto shader_string = rl.loadString(shader_path.string());

   const auto shader_source = NS::String::string(shader_string.data(),NS::ASCIIStringEncoding);
   NS::Error * error = nullptr;
   MTL::Library * defaultLibrary = _device->newLibrary(shader_source, {}, &error);
   if (defaultLibrary==nullptr and error!=nullptr) {
      std::cout << error->localizedDescription()->utf8String() << std::endl;
   }
   game::ensure(defaultLibrary != nullptr,
      std::format("no library ->\n{}",
               (error!=nullptr) ? error->localizedDescription()->utf8String():"")
      );

   auto str = NS::String::string("shadowVertexFunction",NS::ASCIIStringEncoding);
   const MTL::Function *vertexFunction = defaultLibrary->newFunction(str);
   str = NS::String::string("shadowFragmentFunction",NS::ASCIIStringEncoding);
   const MTL::Function *fragmentFunction = defaultLibrary->newFunction(str);
   defaultLibrary->release();

   pipeline_descriptor->setVertexFunction(vertexFunction);
   pipeline_descriptor->setFragmentFunction(fragmentFunction);
   pipeline_descriptor->setSampleCount(1);
   pipeline_descriptor->setDepthAttachmentPixelFormat(_shadowPassTexture->pixelFormat());

   error = nullptr;
   _shadowrps = {
      _device->newRenderPipelineState(pipeline_descriptor.get(),&error),
      [](auto t){t->release();}
   };

   game::ensure(_shadowrps.get() != nullptr,
      std::format("non pipeline descriptor ->\n{}",
               (error!=nullptr)?error->description()->utf8String():"")
      );
   const auto depth_stencil_descriptor = AutoRelease<MTL::DepthStencilDescriptor*,{}>{
      MTL::DepthStencilDescriptor::alloc()->init(),
      [](auto t) {t->release();}
   };
   depth_stencil_descriptor->setDepthCompareFunction(MTL::CompareFunctionLessEqual);
   depth_stencil_descriptor->setDepthWriteEnabled(true);
   _shadowdss = {
      _device->newDepthStencilState(depth_stencil_descriptor.get()),
      [](auto t) {t->release();}
   };

}

auto Renderer::shadowPass(const Camera& camera,
      [[maybe_unused]]CA::MetalDrawable* surface, Scene& scene) const -> void {
   const auto commandQueue = _device->newCommandQueue();
   const auto buffer = commandQueue->commandBuffer();
   const auto renderPassDescriptor = MTL::RenderPassDescriptor::alloc()->init();
   renderPassDescriptor->depthAttachment()->setTexture(_shadowPassTexture.get());
   renderPassDescriptor->depthAttachment()->setLoadAction(MTL::LoadActionClear);
   renderPassDescriptor->depthAttachment()->setStoreAction(MTL::StoreActionStore);
   renderPassDescriptor->depthAttachment()->setClearDepth(1.0f);

   const auto encoder = buffer->renderCommandEncoder(renderPassDescriptor);
   renderPassDescriptor->release();
   scene.setCamera(const_cast<Camera*>(&camera));
   encoder->setRenderPipelineState(_shadowrps.get());
   encoder->setDepthStencilState(_shadowdss.get());
   encoder->setFrontFacingWinding(MTL::WindingClockwise);
   encoder->setCullMode(MTL::CullModeBack);

   encoder->endEncoding();
   scene.render(encoder,RenderPasses::ShadowPass);

   /// buffer->presentDrawable();
   buffer->commit();
   buffer->waitUntilCompleted();

   encoder->release();
   buffer->release();
   commandQueue->release();
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
   scene.setShadowTexture(const_cast<MTL::Texture*>(_shadowPassTexture.get()));
   scene.render(encoder,RenderPasses::MainPass);
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