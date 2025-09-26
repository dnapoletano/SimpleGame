#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>
#include "mesh.hpp"

namespace game {

Mesh::Mesh(MeshData * md)
   : _vertices{md->vertices}, _indexes{md->indexes}
{
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
