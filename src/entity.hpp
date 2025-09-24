#ifndef GAME_TUTORIAL_ENTITY_HPP
#define GAME_TUTORIAL_ENTITY_HPP

/// an entity is just a mesh and a material -> something that is renderable
/// a and a position
#include "material.hpp"
#include "matrix4.hpp"
#include "mesh.hpp"
#include "texture.hpp"
#include "vector3.hpp"

namespace game {

class Entity {
public:
   Entity(Mesh * const mesh, Material * const material, Texture * const texture)
      : _mesh(mesh), _material(material), _texture(texture) {}

   Entity(Mesh * const mesh, Material * const material, Texture * const texture, const Vector3& position)
      : Entity(mesh,material,texture) {
      _model = Matrix4(position);
   }

   Entity(Mesh *const mesh, Material *const material, Texture * const texture,
      const Vector3 &position, const Vector3& axis, const float theta)
      : Entity(mesh,material,texture) {
      const auto translation = Matrix4(position);
      const auto rotation = Matrix4(axis,theta);
      _model = rotation * translation;
   }

   [[nodiscard]] constexpr auto getVertexData() const -> const VertexData*  { return _mesh->getVertexArray().data();}
   [[nodiscard]] constexpr auto getVertexBuffer() const -> MTL::Buffer*  { return _mesh->getVertexBuffer();}
   [[nodiscard]] constexpr auto getIndexBuffer() const -> MTL::Buffer*  { return _mesh->getIndexBuffer();}
   [[nodiscard]] constexpr auto getIndexCount() const -> size_t  { return _mesh->getIndexCount();}
   [[nodiscard]] constexpr auto size() const -> size_t { return _mesh->size();}
   [[nodiscard]] constexpr auto getShaderFunctions() const -> ShaderFunctions {return _material->getShaderFunctions();}
   [[nodiscard]] constexpr auto getRenderPipelineState() const -> MTL::RenderPipelineState * {return _material->getRenderPipelineState();}
   [[nodiscard]] constexpr auto getDepthStencilState() const -> MTL::DepthStencilState* {return _material->getDepthStencilState();}
   [[nodiscard]] constexpr auto getModel() const -> const Matrix4& {return _model;}
   [[nodiscard]] constexpr auto getTexture() const -> MTL::Texture * {return _texture->getTexture();}

   [[nodiscard]] constexpr auto getPrimitive() const -> MTL::PrimitiveType {return _mesh->getPrimitiveType();}
   [[nodiscard]] constexpr auto getVertexCount() const -> size_t {return _mesh->n_verts();}


private:
   Mesh * _mesh{nullptr};
   Material * _material{nullptr};
   Texture * _texture{nullptr};
   Matrix4 _model{};
};

}

#endif // GAME_TUTORIAL_ENTITY_HPP
