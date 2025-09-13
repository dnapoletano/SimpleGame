#ifndef GAME_TUTORIAL_ENTITY_HPP
#define GAME_TUTORIAL_ENTITY_HPP

/// an entity is just a mesh and a material -> something that is renderable
/// a and a position
#include "material.hpp"
#include "matrix4.hpp"
#include "mesh.hpp"
#include "vector3.hpp"

namespace game {

class Entity {
public:
   Entity(Mesh * const mesh, Material * const material)
      : _mesh(mesh), _material(material) {}

   Entity(Mesh * const mesh, Material * const material, const Vector3& position)
      : Entity(mesh,material) {
      const auto translation = Matrix4(position);
      for (auto &[pos, color] : *(_mesh->accessVertexArray())) {
         pos = translation * pos;
      }
   }

   Entity(Mesh *const mesh, Material *const material,
      const Vector3 &position, const Vector3& axis, const float theta)
      : Entity(mesh,material) {
      const auto translation = Matrix4(position);
      const auto rotation = Matrix4(axis,theta);
      for (auto &[pos, color]: *(_mesh->accessVertexArray())) {
         pos = translation * rotation * pos;
      }
   }

   [[nodiscard]] constexpr auto getVertexData() const  { return _mesh->getVertexArray().data();}
   [[nodiscard]] constexpr auto size() const { return _mesh->size();}
   [[nodiscard]] constexpr auto getShaderFunctions() const -> ShaderFunctions {return _material->getShaderFunctions();}


private:
   Mesh * _mesh{nullptr};
   Material * _material{nullptr};
};

}

#endif // GAME_TUTORIAL_ENTITY_HPP
