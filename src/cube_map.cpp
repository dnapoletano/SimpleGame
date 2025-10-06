#include "cube_map.hpp"

#include <cmath>
#include <iostream>
#include <filesystem>

#include "error.hpp"

#include "utils.hpp"

#include "stb_image.h"
namespace game {

CubeMap::CubeMap(const std::vector<std::span<const std::byte>> &faces,
   std::string_view shader,
   const std::uint32_t width, const std::uint32_t height, MTL::Device * device, MeshFactory * mf)
   : _texture{}, _device(device) {

   auto w = static_cast<int>(width);
   auto h = static_cast<int>(height);
   auto num_channels = int{4};

   const auto textureDescriptor = AutoRelease<MTL::TextureDescriptor*>{
      MTL::TextureDescriptor::alloc()->init(),
      [](auto t) {t->release();}
   };
   const auto mipLevels = static_cast<size_t>(std::floor(std::log2(std::max(width,height)))+1);
   textureDescriptor->setArrayLength(1);
   textureDescriptor->setWidth(w);
   textureDescriptor->setHeight(h);
   textureDescriptor->setTextureType(MTL::TextureTypeCube);
   textureDescriptor->setPixelFormat(MTL::PixelFormatRGBA8Unorm);
   textureDescriptor->setUsage(MTL::TextureUsageShaderRead | MTL::TextureUsageRenderTarget);
   textureDescriptor->setStorageMode(MTL::StorageModeShared);
   textureDescriptor->setMipmapLevelCount(mipLevels);

   _texture = {
      _device->newTexture(textureDescriptor.get()),
      [](auto t) {return t;}
   };

   const AutoRelease<MTL::CommandQueue*> commandQueue = {
      _device->newCommandQueue(),
      [](auto t) {t->release();}
   };

   AutoRelease<MTL::CommandBuffer*> commandBuffer = {
      commandQueue->commandBuffer(),
      [](auto t) {t->release();}
   };

   for (const auto &[index, face]: ::enumerate(faces)) {
      const auto raw_data = std::unique_ptr<::stbi_uc,void (*)(void*)>(
         ::stbi_load_from_memory(
            reinterpret_cast<const ::stbi_uc*> (face.data()),
            static_cast<int>(face.size()),
            &w, &h, &num_channels,STBI_rgb_alpha),
         ::stbi_image_free);

      ensure(raw_data!=nullptr,
         "Could not read texture");

      const auto region = MTL::Region{0, 0, 0, width, height, 1};
      const NS::UInteger bytesPerRow = 4 * width;
      _texture->replaceRegion(region, 0, index, raw_data.get(), bytesPerRow,0);
      const AutoRelease<MTL::BlitCommandEncoder*> blitEncoder = {
         commandBuffer->blitCommandEncoder(),
         [](auto t) {t->release();}
      };
      blitEncoder->generateMipmaps(_texture.get());
      blitEncoder->endEncoding();
   }
   commandBuffer->commit();
   commandBuffer->waitUntilCompleted();



   const auto shader_source = NS::String::string(shader.data(),NS::ASCIIStringEncoding);
   NS::Error * error = nullptr;
   MTL::Library * defaultLibrary = _device->newLibrary(shader_source, {}, &error);
   if (defaultLibrary==nullptr and error!=nullptr) {
      std::cout << error->localizedDescription()->utf8String() << std::endl;
   }
   game::ensure(defaultLibrary != nullptr,
      std::format("no library ->\n{}",
               (error!=nullptr) ? error->localizedDescription()->utf8String():"")
      );

   auto str = NS::String::string("vertexMain",NS::ASCIIStringEncoding);
   MTL::Function *vertexFunction = defaultLibrary->newFunction(str);
   str = NS::String::string("fragmentMain",NS::ASCIIStringEncoding);
   MTL::Function *fragmentFunction = defaultLibrary->newFunction(str);
   defaultLibrary->release();
   _shaderFunctions["vertexShader"]   = vertexFunction;
   _shaderFunctions["fragmentShader"] = fragmentFunction;
   _cubeMesh = AutoRelease<Mesh*>{
      new Mesh{mf->getMeshData("cube_map",{})},
      [](auto t) {t->~Mesh();}
   };
}


}