#include "scene.hpp"
#include <filesystem>

#include "cube_map.hpp"
#include "mesh_factory.hpp"
#include "renderer.hpp"
#include "resource_reader.hpp"

#include <assimp/Importer.hpp>
#include <assimp/camera.h>
#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "utils.hpp"

namespace game {
Scene::Scene(MTL::Device* device, CA::MetalLayer* layer)
   : _device(device), _layer(layer) {
   const ResourceLoader resourceLoader{ROOT_DIR};

   auto objdata = resourceLoader.loadBytes( (std::filesystem::path(ASSETS_DIR) /"BlenderRoom"/ "Untitled.gltf").string());

   _loadScene(objdata);
   // for (const auto &u: _unique_meshes) {
   //    u->createBuffers(_device);
   // }
   // for (const auto &m: _unique_materials) {
   //    m->setUpRenderPipeLineState(_layer);
   // }

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

   return;

   // MeshFactory mf{objdata};
   // for (const auto& s: mf.getMeshNames()) {
   //    _unique_meshes.push_back(AutoRelease<Mesh *>{new Mesh{mf.getMeshData(s,objdata)}, [](auto t) {t->~Mesh();}});
   // }
   // // _unique_meshes.push_back(AutoRelease<Mesh *>{new Mesh{mf.getMeshData("",objdata)}, [](auto t) { t->~Mesh(); }});
   // //_unique_meshes.push_back(AutoRelease<Mesh *>{new Mesh{mf.getMeshData("Plane",objdata)}, [](auto t) { t->~Mesh(); }});
   //
   //
   // const auto shader_path = std::filesystem::path(SHADERS_DIR) / "textured.metal";
   // const auto shader_string = resourceLoader.loadString(shader_path.string());
   // _unique_materials.push_back(
   //       AutoRelease<Material *>{new Material{shader_string, _device}, [](auto t) { t->~Material(); }});
   //
   // const auto texture_paths = std::vector<std::filesystem::path>{
   //    std::filesystem::path(ASSETS_DIR) / "rustediron1-alt2-Unreal-Engine" / "rustediron2_basecolor.png",
   //    std::filesystem::path(ASSETS_DIR) / "rustediron1-alt2-Unreal-Engine" / "rustediron2_metallic.png",
   //    std::filesystem::path(ASSETS_DIR) / "rustediron1-alt2-Unreal-Engine" / "rustediron2_roughness.png",
   //    std::filesystem::path(ASSETS_DIR) / "rustediron1-alt2-Unreal-Engine" / "rustediron2_normal.png"
   // };
   //
   // std::vector<std::vector<std::byte>> textures_data;
   // for (const auto& t: texture_paths) {
   //    textures_data.emplace_back(resourceLoader.loadBytes(t.string()));
   // }
   //
   // _unique_textures.push_back(
   //       AutoRelease<Texture *>{new Texture{
   //          textures_data, 2048, 2048, _device}, [](auto t) { t->~Texture(); }});
   //
   // for (const auto &u: _unique_meshes) {
   //    u->createBuffers(_device);
   // }
   // for (const auto &m: _unique_materials) {
   //    m->setUpRenderPipeLineState(_layer);
   // }
   //
   // for (const auto& m: _unique_meshes) {
   //    _entities.emplace_back(m.get(),_unique_materials[0].get(), _unique_textures[0].get(),Vector3{0.0f,0.0f,0.0f});
   // }
   //_entities.emplace_back(_unique_meshes[1].get(), _unique_materials[0].get(),_unique_textures[0].get(),Vector3{0.0f,0.0f,0.0f});

   // for (auto i = 0u; i < 20u; ++i) {
   //    for (auto j = 0u; j < 20u; ++j) {
   //       _entities.emplace_back(_unique_meshes[0].get(),
   //          _unique_materials[0].get(), _unique_textures[0].get(),
   //          Vector3{static_cast<float>(i) * 3.0f-10.f, 0.0f, static_cast<float>(j) * 3.0f-10.0f});
   //    }
   // }


   /// create Light Buffers
   // _ambientLightBuffer = {
   //    _device->newBuffer(&_ambientLight,sizeof(_ambientLight),MTL::StorageModeShared),
   //    [](auto t) { t->release();}
   // };
   //
   // _directionalLightBuffer = {
   //    _device->newBuffer(&_directionalLight,sizeof(_directionalLight),MTL::StorageModeShared),
   //    [](auto t) { t->release();}
   // };
   //
   // _pointLightBuffer = {
   //    _device->newBuffer(&_pointLight,sizeof(_pointLight),MTL::StorageModeShared),
   //    [](auto t) { t->release();}
   // };
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
      //if (e.getTexture() == nullptr) continue;
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
      if (e.getTexture()!=nullptr) {
         encoder->setFragmentTexture(e.getTexture(),0);
      }
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

auto Scene::_loadScene([[maybe_unused]]const std::span<std::byte> data) -> void {
   const auto rl = ResourceLoader{std::filesystem::path(ROOT_DIR) / std::filesystem::path(ASSETS_DIR)/"BlenderRoom"};
   auto importer = ::Assimp::Importer{};
   auto path = (std::filesystem::path(ROOT_DIR) / std::filesystem::path(ASSETS_DIR) /"BlenderRoom"/ "Untitled.gltf").string();
   // const auto scene =
   //    importer.ReadFileFromMemory(data.data(),data.size(),
   //       aiProcess_GenSmoothNormals |
   //       aiProcess_JoinIdenticalVertices |
   //       aiProcess_Triangulate |
   //       aiProcess_SortByPType |
   //       aiProcess_FlipUVs |
   //       aiProcess_CalcTangentSpace |
   //       aiProcess_ImproveCacheLocality);
   const auto scene = importer.ReadFile(path,
      aiProcess_GenSmoothNormals |
      aiProcess_Triangulate |
      aiProcess_JoinIdenticalVertices |
      aiProcess_SortByPType |
      aiProcess_FlipUVs |
      aiProcess_CalcTangentSpace |
      aiProcess_ImproveCacheLocality);

   ensure(scene!=nullptr and not (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE),
      std::format("Could not init scene from data,\n{}",importer.GetErrorString()));

   const auto tofloat4 = [](const ::aiVector3D& v) {return simd::float4{v.x,v.y,v.z,1.0f};};
   const auto tofloat3 = [](const ::aiVector3D& v) {return simd::float3{v.x,v.y,v.z};};
   const auto toMatrix4 = [](const ::aiMatrix4x4& m) {
      simd::float4x4 mat;
      mat.columns[0] = simd::float4{m.a1,m.b1,m.c1,m.d1};
      mat.columns[1] = simd::float4{m.a2,m.b2,m.c2,m.d2};
      mat.columns[2] = simd::float4{m.a3,m.b3,m.c3,m.d3};
      mat.columns[3] = simd::float4{m.a4,m.b4,m.c4,m.d4};
      return Matrix4{mat};
   };


   ///// Load Materials ------------------------------------------------------------------------------------------------
   std::vector<std::pair<std::uint32_t,simd::float4>> matTex(scene->mNumMaterials);
   size_t textures = 0;
   for (const auto foundMaterials = std::span{scene->mMaterials, scene->mMaterials + scene->mNumMaterials};
        const auto &[i,m]: enumerate(foundMaterials)) {
      aiString texturePath;

      /// check if it has material, if so, load the relevant textures, else
      /// use simple shader with no textures: TODO: write the alternative shader.
      /// also, I need to have a link between material and texture

      if (m->GetTexture(aiTextureType_BASE_COLOR, 0, &texturePath) == aiReturn_SUCCESS) {
         const auto shader_path =std::filesystem::path("../../")/ std::filesystem::path(SHADERS_DIR) / "textured.metal";
         const auto shader_string = rl.loadString(shader_path.string());
         _unique_materials.push_back(
               AutoRelease<Material *>{new Material{shader_string, _device}, [](auto t) { t->~Material(); }});

         const auto textureTypes = std::vector{
            ::aiTextureType::aiTextureType_BASE_COLOR,
            ::aiTextureType_METALNESS,
            ::aiTextureType_DIFFUSE_ROUGHNESS,
            ::aiTextureType_NORMALS,
         };
         std::vector<std::vector<std::byte>> texturesData;
         for (const auto& t: textureTypes) {
            if (m->GetTexture(t, 0, &texturePath) == aiReturn_SUCCESS)
            std::println("Loading texture {}", texturePath.C_Str());
            texturesData.emplace_back(rl.loadBytes(texturePath.C_Str()));
         }
         _unique_textures.push_back(
            AutoRelease<Texture *>{new Texture{
               texturesData, 2048, 2048, _device}, [](auto t) { t->~Texture();}
            });
         matTex[i] = {textures++,0};
      } else {
         ::aiColor4D matCol;
         ensure(::aiGetMaterialColor(m, AI_MATKEY_COLOR_DIFFUSE, &matCol) == aiReturn_SUCCESS,
            std::format("Could neither read color nor texture for material {}",m->GetName().C_Str()));

         matTex[i] = {-1,simd::float4{matCol.r,matCol.g,matCol.b,matCol.a}};
         const auto shader_path = std::filesystem::path("../../")/std::filesystem::path(SHADERS_DIR) / "general.metal";
         const auto shader_string = rl.loadString(shader_path.string());
         _unique_materials.push_back(
               AutoRelease<Material *>{new Material{shader_string, _device}, [](auto t) { t->~Material(); }});
      }
   }

   std::println("Loaded Materials");

   //// Load Meshes ----------------------------------------------------------------------------------------------------
   auto processNode = [this,matTex,tofloat3,tofloat4,toMatrix4](this auto const& processNode, const ::aiNode * node, const ::aiScene * scene_, const aiMatrix4x4& parentNodeTransformation) -> void {
      const auto currentTransform = parentNodeTransformation * node->mTransformation;
      for (const auto nodeMeshes = std::span{node->mMeshes, node->mMeshes + node->mNumMeshes};
         const auto &[i,meshNumber] : enumerate(nodeMeshes)) {
         const auto m = scene_->mMeshes[meshNumber];
               const auto positions = std::span{m->mVertices,m->mVertices+m->mNumVertices} |
         std::views::transform(tofloat4) | std::ranges::to<std::vector>();
         const auto normals = std::span{m->mNormals, m->mNormals + m->mNumVertices} | std::views::transform(tofloat3) |
                           std::ranges::to<std::vector>();


         ensure(m->HasTextureCoords(0), "texture coords not available");

         const auto tangents = std::span{m->mTangents, m->mTangents + m->mNumVertices} |
                               std::views::transform(tofloat3) | std::ranges::to<std::vector>();

         const auto bittangents = std::span{m->mBitangents, m->mBitangents + m->mNumVertices} |
                                  std::views::transform(tofloat3) | std::ranges::to<std::vector>();

         /// TODO: use correct color
         const auto materialColor = matTex[m->mMaterialIndex].second;
         const auto colors =
               std::views::repeat(std::array{materialColor[0], materialColor[1], materialColor[2]}, m->mNumVertices) |
               std::views::transform([](auto c) { return simd::float3{c[0], c[1], c[2]}; }) |
               std::ranges::to<std::vector>();

         const auto uvs = std::span{m->mTextureCoords[0], m->mTextureCoords[0] + m->mNumVertices} |
                          std::views::transform([](const auto v) { return simd::float2{v.x, v.y}; }) |
                          std::ranges::to<std::vector>();

         std::vector<std::uint32_t> idxs;
         for (const auto faces = std::span{m->mFaces, m->mFaces + m->mNumFaces}; const auto &f: faces) {
            for (auto j = 0u; j < f.mNumIndices; ++j) {
               idxs.emplace_back(f.mIndices[j]);
            }
         }

         auto md = MeshData{createModelData(positions, normals, tangents, bittangents, colors, uvs), std::move(idxs)};
         _unique_meshes.emplace_back(new Mesh{&md}, [](auto t) { t->~Mesh(); });
         _unique_meshes.back()->createBuffers(_device);
         _unique_materials[m->mMaterialIndex]->setUpRenderPipeLineState(_layer);

         const auto transform = toMatrix4(currentTransform);
         _entities.emplace_back(_unique_meshes.back().get(), _unique_materials[m->mMaterialIndex].get(),
                                matTex[m->mMaterialIndex].first != -1u
                                      ? _unique_textures[matTex[m->mMaterialIndex].first].get()
                                      : nullptr,
                                      transform);


      }

      for (auto i = 0u; i < node->mNumChildren; ++i) {
         processNode(node->mChildren[i], scene_, currentTransform);
      }
   };

   processNode(scene->mRootNode, scene, aiMatrix4x4());

   for (const auto foundLights = std::span<::aiLight*>{scene->mLights,scene->mLights + scene->mNumLights};
      const auto l: foundLights) {
      const auto transform = toMatrix4(scene->mRootNode->FindNode(l->mName)->mTransformation);
      _pointLight.position = (transform*Vector3{simd::float3{l->mPosition.x, l->mPosition.y, l->mPosition.z}}).data();
      _pointLight.colour = simd::float4{l->mColorDiffuse.r,l->mColorDiffuse.g,l->mColorDiffuse.b,1.0f};
      _pointLight.strength = .001f;
      break;
   }
   // for (const auto foundCameras = std::span<::aiCamera*>{scene->mCameras,scene->mCameras + scene->mNumCameras};
   //    const auto c: foundCameras) {
   //
   // }
}



}