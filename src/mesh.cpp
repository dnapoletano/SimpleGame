#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>
#include <Foundation/Foundation.hpp>
#include "mesh.hpp"

namespace game {

Mesh::Mesh()
{
   _vertices.reserve(36);

   _vertices.emplace_back(VertexData{.position = {-0.5, -0.5, 0.5, 1.0  }, .color = {1.0, 0.0, 0.0}});
   _vertices.emplace_back(VertexData{.position = {0.5, -0.5, 0.5, 1.0   }, .color = {1.0, 0.0, 0.0}});
   _vertices.emplace_back(VertexData{.position = {0.5, 0.5, 0.5, 1.0    }, .color = {1.0, 0.0, 0.0}});
   /// 3 -> 2 (2)
   _vertices.emplace_back(VertexData{.position = {-0.5, 0.5, 0.5, 1.0   }, .color = {1.0, 0.0, 0.0}});
   /// 5 -> 0 (0)
   _vertices.emplace_back(VertexData{.position = {0.5, -0.5, -0.5, 1.0  }, .color = {0.0, 1.0, 0.0}});
   _vertices.emplace_back(VertexData{.position = {-0.5, -0.5, -0.5, 1.0 }, .color = {0.0, 1.0, 0.0}});
   _vertices.emplace_back(VertexData{.position = {-0.5, 0.5, -0.5, 1.0  }, .color = {0.0, 1.0, 0.0}});
   _vertices.emplace_back(VertexData{.position = {-0.5, 0.5, -0.5, 1.0  }, .color = {0.0, 1.0, 0.0}});
   _vertices.emplace_back(VertexData{.position = {0.5, 0.5, -0.5, 1.0   }, .color = {0.0, 1.0, 0.0}});

   _indexes = {
      0,1,2,2,3,0,4,5,6,7,8,
      4,3,2,8,8,7,3,5,4,1,1,
      0,5,5,0,3,3,7,5,1,4,8,
      8,2,1
   };

}

Mesh::~Mesh() {
   _vertices.clear();
}

auto Mesh::createBuffers(MTL::Device* const device) -> void {
   ensure(static_cast<bool>(_mesh_buffer) == false,
      "mesh buffer already exists!");
   _mesh_buffer = {
      device->newBuffer(_vertices.data(),size(),{}),
      [](auto t) {t->release();}
   };
   _index_buffer = {
      device->newBuffer(_indexes.data(),sizeof(std::uint32_t)*_indexes.size(),{}),
      [](auto t) {t->release();}
   };
}

}
