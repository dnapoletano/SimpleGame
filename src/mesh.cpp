#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>
#include <Foundation/Foundation.hpp>
#include "mesh.hpp"

namespace game {

Mesh::Mesh()
{
   // _vertices.reserve(8);
    _indexes.reserve(36);
   // Front face (z = +0.5)
   _vertices.emplace_back(VertexData{{-0.5f, -0.5f, 0.5f, 1.0f}, {0.0f, 0.0f}});
   _vertices.emplace_back(VertexData{{0.5f, -0.5f, 0.5f, 1.0f}, {1.0f, 0.0f}});
   _vertices.emplace_back(VertexData{{0.5f, 0.5f, 0.5f, 1.0f}, {1.0f, 1.0f}});
   _vertices.emplace_back(VertexData{{-0.5f, 0.5f, 0.5f, 1.0f}, {0.0f, 1.0f}});

   // Back face (z = -0.5)
   _vertices.emplace_back(VertexData{{0.5f, -0.5f, -0.5f, 1.0f}, {0.0f, 0.0f}});
   _vertices.emplace_back(VertexData{{-0.5f, -0.5f, -0.5f, 1.0f}, {1.0f, 0.0f}});
   _vertices.emplace_back(VertexData{{-0.5f, 0.5f, -0.5f, 1.0f}, {1.0f, 1.0f}});
   _vertices.emplace_back(VertexData{{0.5f, 0.5f, -0.5f, 1.0f}, {0.0f, 1.0f}});

   // Left face (x = -0.5)
   _vertices.emplace_back(VertexData{{-0.5f, -0.5f, -0.5f, 1.0f}, {0.0f, 0.0f}});
   _vertices.emplace_back(VertexData{{-0.5f, -0.5f, 0.5f, 1.0f}, {1.0f, 0.0f}});
   _vertices.emplace_back(VertexData{{-0.5f, 0.5f, 0.5f, 1.0f}, {1.0f, 1.0f}});
   _vertices.emplace_back(VertexData{{-0.5f, 0.5f, -0.5f, 1.0f}, {0.0f, 1.0f}});

   // Right face (x = +0.5)
   _vertices.emplace_back(VertexData{{0.5f, -0.5f, 0.5f, 1.0f}, {0.0f, 0.0f}});
   _vertices.emplace_back(VertexData{{0.5f, -0.5f, -0.5f, 1.0f}, {1.0f, 0.0f}});
   _vertices.emplace_back(VertexData{{0.5f, 0.5f, -0.5f, 1.0f}, {1.0f, 1.0f}});
   _vertices.emplace_back(VertexData{{0.5f, 0.5f, 0.5f, 1.0f}, {0.0f, 1.0f}});

   // Top face (y = +0.5)
   _vertices.emplace_back(VertexData{{-0.5f, 0.5f, 0.5f, 1.0f}, {0.0f, 0.0f}});
   _vertices.emplace_back(VertexData{{0.5f, 0.5f, 0.5f, 1.0f}, {1.0f, 0.0f}});
   _vertices.emplace_back(VertexData{{0.5f, 0.5f, -0.5f, 1.0f}, {1.0f, 1.0f}});
   _vertices.emplace_back(VertexData{{-0.5f, 0.5f, -0.5f, 1.0f}, {0.0f, 1.0f}});

   // Bottom face (y = -0.5)
   _vertices.emplace_back(VertexData{{-0.5f, -0.5f, -0.5f, 1.0f}, {0.0f, 0.0f}});
   _vertices.emplace_back(VertexData{{0.5f, -0.5f, -0.5f, 1.0f}, {1.0f, 0.0f}});
   _vertices.emplace_back(VertexData{{0.5f, -0.5f, 0.5f, 1.0f}, {1.0f, 1.0f}});
   _vertices.emplace_back(VertexData{{-0.5f, -0.5f, 0.5f, 1.0f}, {0.0f, 1.0f}});

   _indexes = {
         0,  1,  2,  2,  3,  0, // Front
         4,  5,  6,  6,  7,  4, // Back
         8,  9,  10, 10, 11, 8, // Left
         12, 13, 14, 14, 15, 12, // Right
         16, 17, 18, 18, 19, 16, // Top
         20, 21, 22, 22, 23, 20 // Bottom
   };


}

Mesh::~Mesh() {
   _vertices.clear();
}

auto Mesh::createBuffers(MTL::Device* const device) -> void {
   ensure(static_cast<bool>(_mesh_buffer) == false,
      "mesh buffer already exists!");
   _mesh_buffer = {
      device->newBuffer(_vertices.data(),size(),MTL::ResourceStorageModeShared),
      [](auto t) {t->release();}
   };
   _index_buffer = {
      device->newBuffer(_indexes.data(),sizeof(std::uint32_t)*_indexes.size(),MTL::ResourceStorageModeShared),
      [](auto t) {t->release();}
   };
}

}
