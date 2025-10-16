#ifndef GAME_TUTORIAL_SCENE_HPP
#define GAME_TUTORIAL_SCENE_HPP

#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>

#include "camera.hpp"
#include "cube_map.hpp"
#include "entity.hpp"
#include "light.hpp"

namespace game {
enum class RenderPasses;
/// Class that contains entities:
/// need to make sure that if it contains multiple copies
/// of the same mesh/material it only instances them once
/// and tricks the renderer to think they are different
/// objects, i.e. by trasforming the vertices appropriately
/// should it contain a camera too?
class Scene {
public:
   Scene(MTL::Device* device, CA::MetalLayer* layer);
   constexpr auto setCamera(Camera* camera) -> void {_camera = camera;}
   auto render(MTL::RenderCommandEncoder * encoder,const RenderPasses renderPass) const -> void;
   auto renderSkyBox(MTL::RenderCommandEncoder * encoder) const -> void;
   [[nodiscard]] constexpr auto getCamera() const -> Camera* {return _camera;}
   [[nodiscard]] constexpr auto getTexture(const size_t& i) const -> MTL::Texture * {return _unique_textures[i].get()->getTexture();}
   constexpr auto setShadowTexture(const MTL::Texture* const texture) {_shadowTexture = texture;}
   constexpr auto updatePointLight(const PointLight& pl ) -> void {
      _pointLight.position = pl.position;
      _pointLight.colour = pl.colour;
      _pointLight.strength = pl.strength;
      memcpy(_pointLightBuffer.get()->contents(),&_pointLight,sizeof(_pointLight));
   }
   [[nodiscard]] constexpr auto getPointLight() const { return _pointLight;}

private:
   std::vector<Entity> _entities;
   std::vector<AutoRelease<Mesh*>> _unique_meshes;
   std::vector<AutoRelease<Material*>>_unique_materials;
   std::vector<AutoRelease<Texture*>>_unique_textures;

   AutoRelease<CubeMap*> _cubemap{};

   AmbientLight _ambientLight{
      .strength = 0.1f,
      .colour = {1.0f,1.0f,1.0f,1.0f}
   };

   DirectionalLight _directionalLight{
      .strength = 0.1f,
      .colour = {1.0f,1.0f,1.0f,1.0f},
      .direction = {-1.0f,-1.0f,-1.0f}
   };

   PointLight _pointLight{
      .strength = 50.0f,
      .colour = {1.0f,1.0f,1.0f,1.0f},
      .position = {2.0f,5.0f,0.0f}
   };

   AutoRelease<MTL::Buffer*> _ambientLightBuffer{};
   AutoRelease<MTL::Buffer*> _directionalLightBuffer{};
   AutoRelease<MTL::Buffer*> _pointLightBuffer{};

   Camera *_camera{nullptr};

   MTL::Device*    _device{nullptr};
   CA::MetalLayer* _layer{nullptr};
   const MTL::Texture* _shadowTexture{nullptr};
};
}
#endif // GAME_TUTORIAL_SCENE_HPP
