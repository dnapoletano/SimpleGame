#include "renderer.hpp"
#include "utils.hpp"

namespace game {
Renderer::Renderer(MTL::Device* device)
   : _device(device){
}

/// TODO: Can I use my own texture class?
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

/// TODO: Can I use my own texture class?
auto Renderer::createRenderPassesTexture(const std::uint32_t width, const std::uint32_t height) -> void {
   _renderPassTextures.resize(6);
   constexpr std::array<MTL::PixelFormat,6> formats = {
      MTL::PixelFormatRGBA16Float,  // positions
      MTL::PixelFormatRGBA16Float,  // normals
      MTL::PixelFormatRGBA8Unorm,   // albedo
      MTL::PixelFormatRGBA8Unorm,   // specular
      MTL::PixelFormatRGBA8Unorm,   // roughness
      MTL::PixelFormatBGRA8Unorm    // final output
  };
   for (auto const& [i, pd]: ::enumerate(_renderPassTextures)) {
      const auto descriptor = AutoRelease<MTL::TextureDescriptor*>{
         MTL::TextureDescriptor::alloc()->init(),
         [](auto t) {t->release();}
      };
      descriptor->setPixelFormat(formats[i]);
      descriptor->setUsage(MTL::TextureUsageRenderTarget | MTL::TextureUsageShaderRead | MTL::TextureUsageShaderWrite);
      descriptor->setSampleCount(1);
      descriptor->setWidth(width);
      descriptor->setHeight(height);
      descriptor->setStorageMode(MTL::StorageModePrivate);
      pd = AutoRelease<MTL::Texture*>{
         _device->newTexture(descriptor.get()),
         [](auto t) {t->release();}
      };
   }
}

auto Renderer::lightingPipelineState() -> void {
   const ResourceLoader rl{ROOT_DIR};
   const auto shader_path = std::filesystem::path(SHADERS_DIR) / "lighting.metal";
   const auto shader_source = NS::String::string(rl.loadString(shader_path.string()).data(), NS::ASCIIStringEncoding);
   NS::Error * error = nullptr;
   MTL::Library *defaultLibrary = _device->newLibrary(shader_source,{},&error);
   if (defaultLibrary==nullptr and error!=nullptr) {
      std::cout << error->localizedDescription()->utf8String() << std::endl;
   }
   game::ensure(defaultLibrary != nullptr,
      std::format("no library ->\n{}",
               (error!=nullptr) ? error->localizedDescription()->utf8String():"")
      );

   const auto str = NS::String::string("mainFunction",NS::ASCIIStringEncoding);
   MTL::Function *mainFunction = defaultLibrary->newFunction(str);
   defaultLibrary->release();
   _lightingcps = AutoRelease<MTL::ComputePipelineState*,{}>{
      _device->newComputePipelineState(mainFunction, &error),
      [](auto t) {t->release();}
   };

   mainFunction->release();
}


/// TODO: the geometry and lighting pass are identical but a couple of things
auto Renderer::geometryPass(const Camera& camera,
      [[maybe_unused]]CA::MetalDrawable* surface, Scene& scene) const -> void {
   const auto commandQueue = _device->newCommandQueue();
   const auto buffer = commandQueue->commandBuffer();

   const auto renderPassDescriptor = MTL::RenderPassDescriptor::alloc()->init();
   for (auto const& [i, pd]: ::enumerate(_renderPassTextures)) {
      const MTL::ClearColor clear_color{0.,0.,0.,1.};
      const auto cd = renderPassDescriptor->colorAttachments()->object(i);
      cd->setTexture(pd.get());
      cd->setLoadAction(MTL::LoadActionClear);
      cd->setClearColor(clear_color);
      cd->setStoreAction(MTL::StoreActionStore);
   }

   renderPassDescriptor->depthAttachment()->setLoadAction(MTL::LoadActionClear);
   renderPassDescriptor->depthAttachment()->setStoreAction(MTL::StoreActionStore);
   renderPassDescriptor->depthAttachment()->setClearDepth(1.0);
   renderPassDescriptor->depthAttachment()->setTexture(_depthTexture.get());
   const auto encoder = buffer->renderCommandEncoder(renderPassDescriptor);

   scene.setCamera(const_cast<Camera*>(&camera));
   scene.render(encoder);
   scene.renderSkyBox(encoder);

   encoder->endEncoding();
   buffer->commit();
   buffer->waitUntilCompleted();

   encoder->release();
   buffer->release();
   commandQueue->release();
}

auto Renderer::render([[maybe_unused]] const Camera& camera,
      [[maybe_unused]] const CA::MetalDrawable* surface,
      [[maybe_unused]] Scene& scene) const -> void {

   const auto commandQueue = _device->newCommandQueue();
   const auto buffer = commandQueue->commandBuffer();

   const auto cEncoder = buffer->computeCommandEncoder();
   cEncoder->setComputePipelineState(_lightingcps.get());

   for (const auto &[i,t] : enumerate(_renderPassTextures)) {
      cEncoder->setTexture(t.get(),i);
   }

   scene.setCamera(const_cast<Camera*>(&camera));
   scene.lightingPass(cEncoder);

   const auto gridSize = MTL::Size(_renderPassTextures[5]->width(),_renderPassTextures[5]->height(),1);
   const auto threadGroupSize = MTL::Size::Make(8,8,1);;
   cEncoder->dispatchThreads(gridSize,threadGroupSize);
   cEncoder->endEncoding();

   const AutoRelease<MTL::BlitCommandEncoder*> blitEncoder = {
      buffer->blitCommandEncoder(),
      [](auto t) {t->release();}
   };
   blitEncoder->copyFromTexture(_renderPassTextures[5].get(), 0, 0,
      MTL::Origin(),
      MTL::Size(_renderPassTextures[5]->width(),_renderPassTextures[5]->height(), 1),
      surface->texture(), 0, 0, MTL::Origin());
   blitEncoder->endEncoding();

   buffer->presentDrawable(surface);
   buffer->commit();
   buffer->waitUntilCompleted();
   buffer->release();
   commandQueue->release();
}

}