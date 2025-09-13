#ifndef GAME_TUTORIAL_MESH_HPP
#define GAME_TUTORIAL_MESH_HPP

#include <simd/simd.h>
#include <vector>
#include <iostream>

#include "auto_release.hpp"
#include "vector3.hpp"

namespace game {

struct VertexData {
   Vector3 position;
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


private:
   std::vector<VertexData> _vertices;
};
}

inline std::ostream& operator<<(std::ostream& oss, const simd::float4 inv) {
   return oss << "(" << inv[0] << ", " << inv[1] << ", "<< inv[2] << ", " << inv[3] << ")";
}

inline std::ostream& operator<<(std::ostream& oss, const simd::half3 inv) {
   return oss << "(" << static_cast<float>(inv[0]) << ", " << static_cast<float>(inv[1]) << ", "<< static_cast<float>(inv[2]) << ")";
}

#endif //GAME_TUTORIAL_MESH_HPP
