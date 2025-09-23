#include "scene.hpp"
#include <filesystem>

#include "resource_reader.hpp"

namespace game {
Scene::Scene(MTL::Device* device, CA::MetalLayer* layer)
   : _device(device), _layer(layer) {
   ResourceLoader resourceLoader{ROOT_DIR};
   _entities.reserve(2);

   _unique_meshes.reserve(1);
   _unique_materials.reserve(1);
   _unique_textures.reserve(1);
   /// Can probably abstract this too
   _unique_meshes.push_back(
      AutoRelease<Mesh*>{new Mesh{},
         [](auto t) {t->~Mesh();}});

   const auto shader_path = std::filesystem::path(SHADERS_DIR) / "textured.metal";
   const auto shader_string = resourceLoader.loadString(shader_path.string());
   _unique_materials.push_back(
      AutoRelease<Material*>{new Material{shader_string,_device},
      [](auto t) {t->~Material();}});

   const auto texture_path = std::filesystem::path(ASSETS_DIR) / "container2.png";
   auto texture_data = resourceLoader.loadBytes(texture_path.string());
   _unique_textures.push_back(
      AutoRelease<Texture*>{
         new Texture{texture_data,500,500,_device},
      [](auto t) {t->~Texture();}});

   for (const auto& u: _unique_meshes) {
      u->createBuffers(_device);
   }
   for (const auto& m: _unique_materials) {
      m->setUpRenderPipeLineState(_layer);
   }

   _entities.emplace_back(_unique_meshes[0].get(),
      _unique_materials[0].get(), _unique_textures[0].get(),
      game::Vector3{0.0f,1.0f,0.0f});
   _entities.emplace_back(_unique_meshes[0].get(),
      _unique_materials[0].get(), _unique_textures[0].get(),
      game::Vector3{0.0f,-1.0f,0.0f});

   _entities.emplace_back(_unique_meshes[0].get(),
   _unique_materials[0].get(), _unique_textures[0].get(),
   game::Vector3{-2.0f,1.0f,0.0f});
   _entities.emplace_back(_unique_meshes[0].get(),
      _unique_materials[0].get(), _unique_textures[0].get(),
      game::Vector3{2.0f,-1.0f,0.0f});

   _entities.emplace_back(_unique_meshes[0].get(),
   _unique_materials[0].get(), _unique_textures[0].get(),
   game::Vector3{0.0f,1.0f,2.0f});
   _entities.emplace_back(_unique_meshes[0].get(),
      _unique_materials[0].get(),_unique_textures[0].get(),
      game::Vector3{0.0f,-1.0f,-2.0f},
      Vector3{1.0f,0.0f,0.0f}, 90.
      );
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

      encoder->drawIndexedPrimitives(e.getPrimitive(), e.getIndexCount(), MTL::IndexTypeUInt32,e.getIndexBuffer(),0);
   }

   encoder->endEncoding();
}


}