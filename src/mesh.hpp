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
   simd::float3 normal;
   simd::float3 tangent;
   simd::float3 bitangent;
   simd::float2 uv;
};

struct MeshData {
   std::vector<VertexData> vertices;
   std::vector<std::uint32_t> indexes;
};

inline auto operator==(const VertexData& v1, const VertexData& v2) -> bool {
   return v1.position.x == v2.position.x and v1.position.y == v2.position.y and v1.position.z == v2.position.z;
}

class Mesh {
public:
   Mesh(MeshData * md);
   ~Mesh() = default;

   [[nodiscard]] auto getVertexArray() const             -> const std::span<VertexData>& {return _vertices;}
   [[nodiscard]] auto accessVertexArray()                -> std::span<VertexData>* {return &_vertices;}
   [[nodiscard]] constexpr auto size() const             -> size_t {return _vertices.size()*sizeof(VertexData);}
   [[nodiscard]] constexpr auto n_verts() const          -> size_t {return _vertices.size();}
   auto createBuffers(MTL::Device* device)               -> void;
   [[nodiscard]] constexpr auto getVertexBuffer() const  -> MTL::Buffer * {return _mesh_buffer.get();}
   [[nodiscard]] constexpr auto getIndexBuffer() const   -> MTL::Buffer * {return _index_buffer.get();}
   [[nodiscard]] constexpr auto getIndexCount() const    -> size_t {return _indexes.size();}
   [[nodiscard]] constexpr auto getPrimitiveType() const -> MTL::PrimitiveType {return _primitiveType;}

private:
   std::span<VertexData> _vertices;
   std::span<std::uint32_t> _indexes;
   AutoRelease<MTL::Buffer*> _mesh_buffer{};
   AutoRelease<MTL::Buffer*> _index_buffer{};
   MTL::PrimitiveType _primitiveType {MTL::PrimitiveTypeTriangle};
};
}

template<>
struct std::formatter<simd::float4> {
   static constexpr auto parse(const std::format_parse_context &ctx) {
      return std::cbegin(ctx);
   }

   static auto format(const simd::float4 &vd, std::format_context &ctx) {
      return std::format_to(ctx.out(),
         "({}, {}, {}, {})", vd.x, vd.y,vd.z, vd.w);
   }
};

template<>
struct std::formatter<simd::float3> {
   static constexpr auto parse(const std::format_parse_context &ctx) {
      return std::cbegin(ctx);
   }

   static auto format(const simd::float3 &vd, std::format_context &ctx) {
      return std::format_to(ctx.out(),
         "({}, {}, {})", vd.x, vd.y, vd.z);
   }
};

template<>
struct std::formatter<simd::float2> {
   static constexpr auto parse(const std::format_parse_context &ctx) {
      return std::cbegin(ctx);
   }

   static auto format(const simd::float2 &vd, std::format_context &ctx) {
      return std::format_to(ctx.out(),
         "({}, {})", vd.x, vd.y);
   }
};

template<>
struct std::formatter<game::VertexData> {
   static constexpr auto parse(const std::format_parse_context &ctx) {
      return std::cbegin(ctx);
   }

   static auto format(const game::VertexData &vd, std::format_context &ctx) {
      return std::format_to(ctx.out(),
         "position = {}, normal = {}, uv = {}", vd.position, vd.normal, vd.uv);
   }
};

template<>
struct std::formatter<game::Mesh> {
   static constexpr auto parse(const std::format_parse_context &ctx) {
      return std::cbegin(ctx);
   }

   static auto format(const game::Mesh &mesh, std::format_context &ctx) {
      return std::format_to(ctx.out(),
         "{}", mesh);
   }
};
#endif //GAME_TUTORIAL_MESH_HPP
