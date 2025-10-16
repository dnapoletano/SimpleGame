#include "scene.hpp"
#include <filesystem>

#include "cube_map.hpp"
#include "mesh_factory.hpp"
#include "renderer.hpp"
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

   auto objdata = resourceLoader.loadBytes( (std::filesystem::path(ASSETS_DIR) / "Suzanne.obj").string());
   _unique_meshes.push_back(AutoRelease<Mesh *>{new Mesh{mf.getMeshData("Suzanne",objdata)}, [](auto t) { t->~Mesh(); }});
   _unique_meshes.push_back(AutoRelease<Mesh *>{new Mesh{mf.getMeshData("Plane",objdata)}, [](auto t) { t->~Mesh(); }});

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

   _entities.emplace_back(_unique_meshes[0].get(),_unique_materials[0].get(), _unique_textures[0].get(),Vector3{0.0f,0.0f,0.0f});
   _entities.emplace_back(_unique_meshes[1].get(), _unique_materials[0].get(),_unique_textures[0].get(),Vector3{0.0f,0.0f,0.0f});

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
   // const auto cubemapShaderPath = (std::filesystem::path(SHADERS_DIR) / "skybox.metal").string();
   //
   // _cubemap = AutoRelease<CubeMap*>{
   //          new CubeMap{
   //             {
   //                resourceLoader.loadBytes((std::filesystem::path(ASSETS_DIR) / "skybox" / "right.jpg").string()),
   //                resourceLoader.loadBytes((std::filesystem::path(ASSETS_DIR) / "skybox" / "left.jpg").string()),
   //                resourceLoader.loadBytes((std::filesystem::path(ASSETS_DIR) / "skybox" / "top.jpg").string()),
   //                resourceLoader.loadBytes((std::filesystem::path(ASSETS_DIR) / "skybox" / "bottom.jpg").string()),
   //                resourceLoader.loadBytes((std::filesystem::path(ASSETS_DIR) / "skybox" / "front.jpg").string()),
   //                resourceLoader.loadBytes((std::filesystem::path(ASSETS_DIR) / "skybox" / "back.jpg").string())
   //             },
   //             resourceLoader.loadString(cubemapShaderPath),
   //             2048u,2048u, _device, &mf
   //          },
   //       [](auto t){t->~CubeMap();}
   // };
   // _cubemap->setUpRenderPipeLineState(_layer);
   // _cubemap->createBuffers(_device);


}

auto Scene::render(MTL::RenderCommandEncoder *encoder, const RenderPasses renderPass) const -> void {
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

      const auto orthoProj = Matrix4::orthografic(-10.0f,10.0f,-10.0f,10.0f,-25.0f,25.0f).data();
      const auto cameraLightView = Matrix4::lookAt(-_pointLight.position,{0.0f,0.0f,0.0f},{0.0f,1.0f,0.0f}).data();
      const auto lightViewProjMatrix = orthoProj * cameraLightView;

      switch (renderPass) {
         case RenderPasses::MainPass:
            default: {
            encoder->setVertexBytes(_camera->getData(),_camera->size(),2);
            break;
         }
         case RenderPasses::ShadowPass: {
            encoder->setVertexBytes(&lightViewProjMatrix, sizeof(lightViewProjMatrix),2);
            break;
         }
      }
      encoder->setFragmentTexture(e.getTexture(),0);
      if (_cubemap.get() == nullptr) {
         encoder->setFragmentTexture(nullptr,1);
      } else {
         encoder->setFragmentTexture(_cubemap->getTextures(),1);
      }
      encoder->setFragmentTexture(_shadowTexture,2);
      /// could compress this into a unique buffer with offsets?
      encoder->setFragmentBuffer(_ambientLightBuffer.get(),0,3);
      encoder->setFragmentBuffer(_directionalLightBuffer.get(),0,4);
      encoder->setFragmentBuffer(_pointLightBuffer.get(),0,5);

      encoder->setFragmentBytes(&_camera->getPosition().data(),sizeof(_camera->getPosition().data()),6);
      encoder->setFragmentBytes(&lightViewProjMatrix,sizeof(lightViewProjMatrix),7);
      encoder->drawIndexedPrimitives(
         e.getPrimitive(),
         e.getIndexCount(),
         MTL::IndexTypeUInt32,
         e.getIndexBuffer(),
         0);
   }
}

auto Scene::renderSkyBox(MTL::RenderCommandEncoder * encoder) const -> void {
   if (_cubemap.get() == nullptr) {
      return;
   }
   MTL::DepthStencilDescriptor* dsd = MTL::DepthStencilDescriptor::alloc()->init();
   dsd->setDepthWriteEnabled(false);
   dsd->setDepthCompareFunction(MTL::CompareFunctionLessEqual);
   const auto depthState = _device->newDepthStencilState(dsd);
   dsd->release();
   encoder->setDepthStencilState(depthState);
   depthState->release();

   encoder->setRenderPipelineState(_cubemap->getRenderPipelineState());
   encoder->setFrontFacingWinding(MTL::WindingClockwise);
   encoder->setCullMode(MTL::CullModeFront);
   encoder->setVertexBuffer(_cubemap->getVertexBuffer(),0,0);
   const auto model = _cubemap->getModel().data();
   encoder->setVertexBytes(&(model),sizeof(model),1);
   const auto lookat = _camera->getLookAt().data();
   const auto proj = _camera->getProj().data();
   encoder->setVertexBytes(&lookat,sizeof(lookat),2);
   encoder->setVertexBytes(&proj,sizeof(proj),3);
   encoder->setFragmentTexture(_cubemap->getTextures(),0);

   encoder->drawIndexedPrimitives(_cubemap->getPrimitive(),
      _cubemap->getIndexCount(),
      MTL::IndexTypeUInt32,
      _cubemap->getIndexBuffer(),
      0);

}


}