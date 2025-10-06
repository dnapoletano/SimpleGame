#ifndef GAME_TUTORIAL_MESH_FACTORY_HPP
#define GAME_TUTORIAL_MESH_FACTORY_HPP

#include <cstddef>

#include <span>
#include <string>
#include <ranges>
#include <algorithm>
#include <string_view>

#include "auto_release.hpp"
#include "mesh.hpp"
#include "resource_reader.hpp"

namespace game {

template<class... Args>
std::vector<VertexData> createModelData(Args && ...args) {
   return std::views::zip(std::forward<Args>(args)...) |
      std::views::transform([](auto && tuple) {
         return std::apply([]<class... A>(A && ...a) {
            return VertexData{std::forward<A>(a)...};
         },tuple);} ) |
         std::ranges::to<std::vector>();
}

struct StringHash {
   using is_transparent = void;
   auto operator()(const char *str) const -> size_t {
      return std::hash<std::string_view>{}(str);
   }
   auto operator()(const std::string_view str) const -> size_t {
      return std::hash<std::string_view>{}(str);
   }
   auto operator()(const std::string& str) const -> size_t {
      return std::hash<std::string_view>{}(str);
   }
};

template <class T>
using StringMap = std::unordered_map<std::string, T, StringHash, std::equal_to<>>;

/// reads or creates meshes and owns them, then distribute them to
/// entities (might need an entity factory as well)

class MeshFactory {
public:
   MeshFactory() = default;
   /// certainly non-copyable
   MeshFactory(const MeshFactory&) = delete;
   MeshFactory(MeshFactory&&)      = delete;

   [[nodiscard]] auto getMeshData(std::string_view mesh_name, std::span<std::byte>) -> MeshData *;


private:
   /// Primitive meshes
   static auto _cube(const float& length)          -> MeshData;
   static auto _cubeMap()                          -> MeshData;
   static auto _sphere(const float& radius)        -> MeshData;

   //an unordered map of mesh and a name
   StringMap<MeshData> _loadedMeshes;
};
}

#endif // GAME_TUTORIAL_MESH_FACTORY_HPP
