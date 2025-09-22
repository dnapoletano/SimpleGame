#include "texture.hpp"

#include "error.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <memory>

namespace game {

Texture::Texture(const std::span<std::byte> data, const size_t width, const size_t height, MTL::Device * device) {
   auto w = static_cast<int>(width);
   auto h = static_cast<int>(height);
   auto num_channels = int{4};

   const auto raw_data = std::unique_ptr<::stbi_uc,void (*)(void*)>(
      ::stbi_load_from_memory(
         reinterpret_cast<const ::stbi_uc*> (data.data()),
         static_cast<int>(data.size()),
         &w, &h, &num_channels,STBI_rgb_alpha),
      ::stbi_image_free);

   ensure(raw_data!=nullptr,
      "Could not read texture");

   const auto textureDescriptor = AutoRelease<MTL::TextureDescriptor*>{
      MTL::TextureDescriptor::alloc()->init(),
      [](auto t) {t->release();}
   };

   textureDescriptor->setWidth(w);
   textureDescriptor->setHeight(h);
   textureDescriptor->setPixelFormat(MTL::PixelFormatRGBA8Unorm);
   textureDescriptor->setUsage(MTL::TextureUsageShaderRead | MTL::TextureUsageRenderTarget);
   textureDescriptor->setStorageMode(MTL::StorageModeShared);

   _texture = {
      device->newTexture(textureDescriptor.get()),
      [](auto t) {return t;}
   };

   const auto region = MTL::Region{0, 0, 0, width, height, 1};

   const NS::UInteger bytesPerRow = 4 * width;
   _texture->replaceRegion(region, 0, raw_data.get(), bytesPerRow);


}

}