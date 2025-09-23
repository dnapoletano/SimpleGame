#include "texture.hpp"

#include "error.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <memory>
#include <simd/simd.h>

namespace game {

Texture::Texture(const std::vector<std::vector<std::byte>>& datavec,
   const size_t width,
   const size_t height,
   MTL::Device * device) {

   auto w = static_cast<int>(width);
   auto h = static_cast<int>(height);
   auto num_channels = int{4};

   const auto textureDescriptor = AutoRelease<MTL::TextureDescriptor*>{
      MTL::TextureDescriptor::alloc()->init(),
      [](auto t) {t->release();}
   };
   const auto mipLevels = std::floor(std::log2(std::max(width,height)))+1;
   textureDescriptor->setArrayLength(datavec.size());
   textureDescriptor->setWidth(w);
   textureDescriptor->setHeight(h);
   textureDescriptor->setTextureType(MTL::TextureType2DArray);
   textureDescriptor->setPixelFormat(MTL::PixelFormatRGBA8Unorm);
   textureDescriptor->setUsage(MTL::TextureUsageShaderRead | MTL::TextureUsageRenderTarget);
   textureDescriptor->setStorageMode(MTL::StorageModeShared);
   textureDescriptor->setMipmapLevelCount(mipLevels);

   _texture = {
      device->newTexture(textureDescriptor.get()),
      [](auto t) {return t;}
   };

   const AutoRelease<MTL::CommandQueue*> commandQueue = {
      device->newCommandQueue(),
      [](auto t) {t->release();}
   };

   AutoRelease<MTL::CommandBuffer*> commandBuffer = {
      commandQueue->commandBuffer(),
      [](auto t) {t->release();}
   };

   for (auto i = 0u; i < datavec.size(); ++i) {
      const auto& data = datavec[i];
      const auto raw_data = std::unique_ptr<::stbi_uc,void (*)(void*)>(
         ::stbi_load_from_memory(
            reinterpret_cast<const ::stbi_uc*> (data.data()),
            static_cast<int>(data.size()),
            &w, &h, &num_channels,STBI_rgb_alpha),
         ::stbi_image_free);

      ensure(raw_data!=nullptr,
         "Could not read texture");

      const auto region = MTL::Region{0, 0, 0, width, height, 1};
      const NS::UInteger bytesPerRow = 4 * width;
      _texture->replaceRegion(region, 0, i, raw_data.get(), bytesPerRow,0);
      const AutoRelease<MTL::BlitCommandEncoder*> blitEncoder = {
         commandBuffer->blitCommandEncoder(),
         [](auto t) {t->release();}
      };
      blitEncoder->generateMipmaps(_texture.get());
      blitEncoder->endEncoding();
   }
   commandBuffer->commit();
   commandBuffer->waitUntilCompleted();
}

}