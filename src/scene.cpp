#include "scene.hpp"

namespace game {
Scene::Scene(MTL::Device* device, CA::MetalLayer* layer)
   : _device(device), _layer(layer) {

   _entities.reserve(2);

   _unique_meshes.reserve(1);
   _unique_materials.reserve(1);
   /// Can probably abstract this too
   _unique_meshes.push_back(
      AutoRelease<Mesh*>{new Mesh{},
         [](auto t) {t->~Mesh();}});
   _unique_materials.push_back(
      AutoRelease<Material*>{new Material{"../../shaders/general.metal",_device},
      [](auto t) {t->~Material();}});

   for (const auto& u: _unique_meshes) {
      u->createBuffers(_device);
   }
   for (const auto& m: _unique_materials) {
      m->setUpRenderPipeLineState(_layer);
   }

   _entities.emplace_back(_unique_meshes[0].get(),
      _unique_materials[0].get(),
      game::Vector3{0.0f,1.0f,0.0f});
   _entities.emplace_back(_unique_meshes[0].get(),
      _unique_materials[0].get(),
      game::Vector3{0.0f,-1.0f,0.0f});

   _entities.emplace_back(_unique_meshes[0].get(),
   _unique_materials[0].get(),
   game::Vector3{-2.0f,1.0f,0.0f});
   _entities.emplace_back(_unique_meshes[0].get(),
      _unique_materials[0].get(),
      game::Vector3{2.0f,-1.0f,0.0f});

   _entities.emplace_back(_unique_meshes[0].get(),
   _unique_materials[0].get(),
   game::Vector3{0.0f,1.0f,2.0f});
   _entities.emplace_back(_unique_meshes[0].get(),
      _unique_materials[0].get(),
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
      ensure(_camera != nullptr, "Camera not setup for render");
      encoder->setVertexBytes(_camera->getData(),_camera->size(),2);

      encoder->drawPrimitives(e.getPrimitive(),NS::UInteger{0},
         NS::UInteger{e.getVertexCount()});
   }

   encoder->endEncoding();
}


}