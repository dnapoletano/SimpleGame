#ifndef GAME_TUTORIAL_MESH_HPP
#define GAME_TUTORIAL_MESH_HPP
#include <Metal/Metal.hpp>
#include <simd/simd.h>
#include <vector>
#include <iostream>

#include "auto_release.hpp"
#include "vector3.hpp"

namespace game {

struct VertexData {
   simd::float4 position;
   simd::half3 color;
};

class Mesh {
public:
   Mesh();
   ~Mesh();

   [[nodiscard]] auto getVertexArray() const -> const std::vector<VertexData>& {return _vertices;}
   [[nodiscard]] auto accessVertexArray() -> std::vector<VertexData>* {return &_vertices;}
   [[nodiscard]] constexpr auto size() const -> size_t {return _vertices.size()*sizeof(VertexData);}
   [[nodiscard]] constexpr auto n_verts() const -> size_t {return _vertices.size();}
   auto createBuffers(MTL::Device* device) -> void;
   [[nodiscard]] constexpr auto getVertexBuffer() const -> MTL::Buffer * {return _mesh_buffer.get();}
   [[nodiscard]] constexpr auto getPrimitiveType() const -> MTL::PrimitiveType {return _primitiveType;}

private:
   std::vector<VertexData> _vertices;
   AutoRelease<MTL::Buffer*> _mesh_buffer{};
   MTL::PrimitiveType _primitiveType {MTL::PrimitiveTypeTriangle};
};
}

inline std::ostream& operator<<(std::ostream& oss, const simd::float4 inv) {
   return oss << "(" << inv[0] << ", " << inv[1] << ", "<< inv[2] << ", " << inv[3] << ")";
}

inline std::ostream& operator<<(std::ostream& oss, const simd::half3 inv) {
   return oss << "(" << static_cast<float>(inv[0]) << ", " << static_cast<float>(inv[1]) << ", "<< static_cast<float>(inv[2]) << ")";
}

#endif //GAME_TUTORIAL_MESH_HPP
