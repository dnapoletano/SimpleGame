#include "mesh.hpp"

namespace game {

Mesh::Mesh()
{
   _vertices.reserve(6);
   _vertices.emplace_back(VertexData{
      .position = {-0.75, -0.75, 1, 1.0},
      .color = {1.0, 0.0, 0.0}});
   _vertices.emplace_back(VertexData{
      .position = {-0.75, 0.75, 1, 1.0},
      .color = {0.0, 1.0, 0.0}});
   _vertices.emplace_back(VertexData{
      .position = {0.75, 0.75, 1, 1.0},
      .color = {0.0, 0.0, 1.0}});
   _vertices.emplace_back(VertexData{
      .position = {-0.75, -0.75, 1, 1.0},
      .color = {1.0, 0.0, 0.0}
   });
   _vertices.emplace_back(VertexData{
      .position = {0.75, 0.75, 1, 1.0},
      .color = {0.0, 0.0, 1.0}
   });
   _vertices.emplace_back(VertexData{
      .position = {0.75, -0.75, 1, 1.0},
      .color = {0.0, 1.0, 0.0}
   });
}

Mesh::~Mesh() {
   _vertices.clear();
}

}
