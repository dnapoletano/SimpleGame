#ifndef GAME_TUTORIAL_SCENE_HPP
#define GAME_TUTORIAL_SCENE_HPP

#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>

#include "camera.hpp"
#include "entity.hpp"
#include "light.hpp"

namespace game {
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
   auto render(MTL::RenderCommandEncoder * encoder) -> void;
   [[nodiscard]] constexpr auto getCamera() const -> Camera* {return _camera;}
   [[nodiscard]] constexpr auto getTexture(const size_t& i) const -> MTL::Texture * {return _unique_textures[i].get()->getTexture();}

private:
   std::vector<Entity> _entities;
   std::vector<AutoRelease<Mesh*>> _unique_meshes;
   std::vector<AutoRelease<Material*>>_unique_materials;
   std::vector<AutoRelease<Texture*>>_unique_textures;

   [[maybe_unused]] AmbientLight _ambientLight{
      .strenght = 0.3f,
      .colour = {1.0f,1.0f,1.0f,1.0f}
   };

   [[maybe_unused]] DirectionalLight _directionalLight{
      .strenght = 0.5,
      .colour = {1.0f,1.0f,1.0f,1.0f},
      .direction = {-1.0f,-1.0f,-1.0f}
   };

   PointLight _pointLight{
      .strenght = 50.0f,
      .colour = {1.0f,1.0f,1.0f,1.0f},
      .position = {7.0f,2.5f,0.0f}
   };

   AutoRelease<MTL::Buffer*> _ambientLightBuffer{};
   AutoRelease<MTL::Buffer*> _directionalLightBuffer{};
   AutoRelease<MTL::Buffer*> _pointLightBuffer{};

   Camera *_camera{nullptr};

   MTL::Device*    _device{nullptr};
   CA::MetalLayer* _layer{nullptr};
};
}
#endif // GAME_TUTORIAL_SCENE_HPP
