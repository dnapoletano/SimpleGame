#include "scene.hpp"
#include <filesystem>

#include "mesh_factory.hpp"
#include "resource_reader.hpp"

namespace game {
Scene::Scene(MTL::Device* device, CA::MetalLayer* layer)
   : _device(device), _layer(layer) {
   const ResourceLoader resourceLoader{ROOT_DIR};
   _entities.reserve(2);

   _unique_meshes.reserve(1);
   _unique_materials.reserve(1);
   _unique_textures.reserve(1);


   /// Can probably abstract this too
   MeshFactory mf{};
   //_unique_meshes.push_back(AutoRelease<Mesh *>{new Mesh{mf.getMeshData("cube",{})}, [](auto t) { t->~Mesh(); }});

   auto objdata = resourceLoader.loadBytes( (std::filesystem::path(ASSETS_DIR) / "Sphere.obj").string());
   _unique_meshes.push_back(AutoRelease<Mesh *>{new Mesh{mf.getMeshData("bSphere",objdata)}, [](auto t) { t->~Mesh(); }});

   const auto shader_path = std::filesystem::path(SHADERS_DIR) / "textured.metal";
   const auto shader_string = resourceLoader.loadString(shader_path.string());
   _unique_materials.push_back(
         AutoRelease<Material *>{new Material{shader_string, _device}, [](auto t) { t->~Material(); }});

   const auto texture_paths = std::vector<std::filesystem::path>{
      std::filesystem::path(ASSETS_DIR) / "rustediron1-alt2-Unreal-Engine" / "rustediron2_basecolor.png",
      std::filesystem::path(ASSETS_DIR) / "rustediron1-alt2-Unreal-Engine" / "rustediron2_metallic.png",
      std::filesystem::path(ASSETS_DIR) / "rustediron1-alt2-Unreal-Engine" / "rustediron2_roughness.png",
      std::filesystem::path(ASSETS_DIR) / "rustediron1-alt2-Unreal-Engine" / "rustediron2_normal.png"
   };

   std::vector<std::vector<std::byte>> textures_data;
   for (const auto& t: texture_paths) {
      textures_data.emplace_back(resourceLoader.loadBytes(t.string()));
   }

   _unique_textures.push_back(
         AutoRelease<Texture *>{new Texture{
            textures_data, 2048, 2048, _device}, [](auto t) { t->~Texture(); }});

   for (const auto &u: _unique_meshes) {
      u->createBuffers(_device);
   }
   for (const auto &m: _unique_materials) {
      m->setUpRenderPipeLineState(_layer);
   }

   _entities.emplace_back(_unique_meshes[0].get(),_unique_materials[0].get(), _unique_textures[0].get());

   // for (auto i = 0u; i < 20u; ++i) {
   //    for (auto j = 0u; j < 20u; ++j) {
   //       _entities.emplace_back(_unique_meshes[0].get(),
   //          _unique_materials[0].get(), _unique_textures[0].get(),
   //          Vector3{static_cast<float>(i) * 3.0f-10.f, 0.0f, static_cast<float>(j) * 3.0f-10.0f});
   //    }
   // }


   /// create Light Buffers
   _ambientLightBuffer = {
      _device->newBuffer(&_ambientLight,sizeof(_ambientLight),MTL::StorageModeShared),
      [](auto t) { t->release();}
   };

   _directionalLightBuffer = {
      _device->newBuffer(&_directionalLight,sizeof(_directionalLight),MTL::StorageModeShared),
      [](auto t) { t->release();}
   };

   _pointLightBuffer = {
      _device->newBuffer(&_pointLight,sizeof(_pointLight),MTL::StorageModeShared),
      [](auto t) { t->release();}
   };
}

auto Scene::render(MTL::RenderCommandEncoder *encoder) const -> void {
   for (const auto& e: _entities) {
      encoder->setRenderPipelineState(
         e.getRenderPipelineState());
      encoder->setVertexBuffer(e.getVertexBuffer(),0,0);
      encoder->setFrontFacingWinding(MTL::WindingCounterClockwise);
      encoder->setCullMode(MTL::CullModeBack);
      encoder->setDepthStencilState(e.getDepthStencilState());
      encoder->setVertexBytes(&e.getModel().data(),sizeof(e.getModel().data()),1);
      ensure(_camera != nullptr,
         "Camera not setup for render");
      encoder->setVertexBytes(_camera->getData(),_camera->size(),2);
      encoder->setFragmentTexture(e.getTexture(),0);

      /// could compress this into a unique buffer with offsets?
      encoder->setFragmentBuffer(_ambientLightBuffer.get(),0,1);
      encoder->setFragmentBuffer(_directionalLightBuffer.get(),0,2);
      encoder->setFragmentBuffer(_pointLightBuffer.get(),0,3);

      encoder->setFragmentBytes(&_camera->getPosition().data(),sizeof(_camera->getPosition().data()),4);

      encoder->drawIndexedPrimitives(
         e.getPrimitive(),
         e.getIndexCount(),
         MTL::IndexTypeUInt32,
         e.getIndexBuffer(),
         0);
   }

   encoder->endEncoding();
}


}