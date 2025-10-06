#include "mesh_factory.hpp"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>



namespace game {

auto MeshFactory::getMeshData(const std::string_view mesh_name, [[maybe_unused]] const std::span<std::byte> data) -> MeshData * {
   /// look up if we altrady loaded the mesh
   if (const auto mesh_it = _loadedMeshes.find(mesh_name); mesh_it != std::ranges::cend(_loadedMeshes)) {
      return &mesh_it->second;
   }

   if (mesh_name == "cube") {
      const auto [fst, snd] = _loadedMeshes.emplace("cube", _cube(1.0f));
      return &fst->second;
   } else if (mesh_name == "sphere") {
      const auto [fst, snd] = _loadedMeshes.emplace("sphere", _sphere(1.0f));
      return &fst->second;
   } else if (mesh_name == "cube_map") {
      const auto [fst, snd] = _loadedMeshes.emplace("cube_map", _cubeMap());
      return &fst->second;
   }


   auto importer = ::Assimp::Importer{};
   const auto scene =
      importer.ReadFileFromMemory(data.data(),data.size(),::aiPostProcessSteps::
         aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

   ensure(scene!=nullptr and not (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE),
      "Could not init scene from data");


   for (const auto loadedMeshes = std::span<::aiMesh *>{scene->mMeshes, scene->mMeshes + scene->mNumMeshes};
        const auto& m: loadedMeshes) {
      if (m->mName.C_Str() == mesh_name) {
         std::println("Found Mesh {}", m->mName.C_Str());

         const auto tofloat4 = [](const ::aiVector3D& v) {return simd::float4{v.x,v.y,v.z,1.0f};};
         const auto tofloat3 = [](const ::aiVector3D& v) {return simd::float3{v.x,v.y,v.z};};

         const auto positions = std::span{m->mVertices,m->mVertices+m->mNumVertices} |
            std::views::transform(tofloat4) | std::ranges::to<std::vector>();
         const auto normals = std::span{m->mNormals,m->mNormals+m->mNumVertices} |
            std::views::transform(tofloat3) | std::ranges::to<std::vector>();

         ensure(m->HasTextureCoords(0),"texture coords not available");

         const auto tangents = std::span{m->mTangents, m->mTangents+m->mNumVertices} |
            std::views::transform(tofloat3) | std::ranges::to<std::vector>();

         const auto bittangents = std::span{m->mBitangents, m->mBitangents+m->mNumVertices} |
            std::views::transform(tofloat3) | std::ranges::to<std::vector>();

         const auto uvs = std::span{m->mTextureCoords[0],m->mTextureCoords[0]+m->mNumVertices} |
            std::views::transform([](const auto v) {return simd::float2{v.x,v.y};}) | std::ranges::to<std::vector>();

         std::vector<std::uint32_t> idxs;
         for (const auto faces = std::span{m->mFaces, m->mFaces + m->mNumFaces}; const auto& f : faces) {
            for (auto j = 0u; j < f.mNumIndices; ++j) {
               idxs.emplace_back(f.mIndices[j]);
            }
         }
         const auto [fst, snd] = _loadedMeshes.emplace(m->mName.C_Str(),
            MeshData{createModelData(positions,normals,tangents,bittangents,uvs),std::move(idxs)});
         return &fst->second;
      }
   }
   return nullptr;
}

auto MeshFactory::_cube(const float& length = 1.0f)   -> MeshData {

   const simd::float4 positions[] = {
         {-length/2.0f, -length/2.0f, length/2.0f, 1.0f}, {length/2.0f, -length/2.0f, length/2.0f, 1.0f},  {length/2.0f, length/2.0f, length/2.0f, 1.0f},
         {-length/2.0f, length/2.0f, length/2.0f, 1.0f},  {length/2.0f, -length/2.0f, -length/2.0f, 1.0f}, {-length/2.0f, -length/2.0f, -length/2.0f, 1.0f},
         {-length/2.0f, length/2.0f, -length/2.0f, 1.0f}, {length/2.0f, length/2.0f, -length/2.0f, 1.0f},  {-length/2.0f, -length/2.0f, -length/2.0f, 1.0f},
         {-length/2.0f, -length/2.0f, length/2.0f, 1.0f}, {-length/2.0f, length/2.0f, length/2.0f, 1.0f},  {-length/2.0f, length/2.0f, -length/2.0f, 1.0f},
         {length/2.0f, -length/2.0f, length/2.0f, 1.0f},  {length/2.0f, -length/2.0f, -length/2.0f, 1.0f}, {length/2.0f, length/2.0f, -length/2.0f, 1.0f},
         {length/2.0f, length/2.0f, length/2.0f, 1.0f},   {-length/2.0f, length/2.0f, length/2.0f, 1.0f},  {length/2.0f, length/2.0f, length/2.0f, 1.0f},
         {length/2.0f, length/2.0f, -length/2.0f, 1.0f},  {-length/2.0f, length/2.0f, -length/2.0f, 1.0f}, {-length/2.0f, -length/2.0f, -length/2.0f, 1.0f},
         {length/2.0f, -length/2.0f, -length/2.0f, 1.0f}, {length/2.0f, -length/2.0f, length/2.0f, 1.0f},  {-length/2.0f, -length/2.0f, length/2.0f, 1.0f}};


   constexpr simd::float3 normals[] = {
         {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f, 1.0f},
         {0.0f, 0.0f, 1.0f},  {0.0f, 0.0f, 1.0f},  {0.0f, 0.0f, 1.0f},  {-1.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f},
         {-1.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f},  {1.0f, 0.0f, 0.0f},  {1.0f, 0.0f, 0.0f},
         {1.0f, 0.0f, 0.0f},  {0.0f, 1.0f, 0.0f},  {0.0f, 1.0f, 0.0f},  {0.0f, 1.0f, 0.0f},  {0.0f, 1.0f, 0.0f},
         {0.0f, -1.0f, 0.0f}, {0.0f, -1.0f, 0.0f}, {0.0f, -1.0f, 0.0f}, {0.0f, -1.0f, 0.0f}};

   constexpr simd::float2 uvs[] = {{0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}, {0.0f, 0.0f},
                                              {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}, {0.0f, 0.0f}, {1.0f, 0.0f},
                                              {1.0f, 1.0f}, {0.0f, 1.0f}, {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f},
                                              {0.0f, 1.0f}, {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f},
                                              {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}};


   auto indexes = std::vector<std::uint32_t>{
         0,  1,  2,  2,  3,  0, // Front
         4,  5,  6,  6,  7,  4, // Back
         8,  9,  10, 10, 11, 8, // Left
         12, 13, 14, 14, 15, 12, // Right
         16, 17, 18, 18, 19, 16, // Top
         20, 21, 22, 22, 23, 20 // Bottom
   };
   return MeshData{createModelData(positions,normals,normals,normals,uvs),std::move(indexes)};
}

auto MeshFactory::_cubeMap() -> MeshData {
   constexpr simd::float4 positions[] = {
      {-1.0f,-1.0f,1.0f,1.0f},
      {1.0f,-1.0f,1.0f,1.0f},
      {1.0f,1.0f,1.0f,1.0f},
      {-1.0f,1.0f,1.0f,1.0f},
      {-1.0f,-1.0f,-1.0f,1.0f},
      {1.0f,-1.0f,-1.0f,1.0f},
      {1.0f,1.0f,-1.0f,1.0f},
      {-1.0f,1.0f,-1.0f,1.0f},
   };
   constexpr simd::float3 normals[] = {
         // Front
         { 0,  0, -1}, { 0,  0, -1}, { 0,  0, -1}, { 0,  0, -1},
         // Back
         { 0,  0,  1}, { 0,  0,  1}, { 0,  0,  1}, { 0,  0,  1},
         // Right
         { -1, 0,  0}, { -1, 0,  0}, { -1, 0,  0}, { -1, 0,  0},
         // Left
         { 1,  0,  0}, { 1,  0,  0}, { 1,  0,  0}, { 1,  0,  0},
         // Top
         { 0, -1, 0}, { 0, -1, 0}, { 0, -1, 0}, { 0, -1, 0},
         // Bottom
         { 0,  1, 0}, { 0,  1, 0}, { 0,  1, 0}, { 0,  1, 0}
   };

   constexpr simd::float2 uvs[] = {
      // Front
      {0, 0}, {1, 0}, {1, 1}, {0, 1},
      // Back
      {1, 0}, {0, 0}, {0, 1}, {1, 1},
      // Right
      {0, 0}, {1, 0}, {1, 1}, {0, 1},
      // Left
      {1, 0}, {0, 0}, {0, 1}, {1, 1},
      // Top
      {0, 0}, {1, 0}, {1, 1}, {0, 1},
      // Bottom
      {0, 1}, {1, 1}, {1, 0}, {0, 0},
  };

   auto indexes = std::vector<std::uint32_t>{
      // Front face
      0, 2, 1,  0, 3, 2,
      // Right face
      1, 6, 5,  1, 2, 6,
      // Back face
      5, 7, 4,  5, 6, 7,
      // Left face
      4, 3, 0,  4, 7, 3,
      // Top face
      3, 6, 2,  3, 7, 6,
      // Bottom face
      4, 1, 5,  4, 0, 1
   };
   return MeshData{createModelData(positions,normals,normals,normals,uvs),std::move(indexes)};
}



auto MeshFactory::_sphere([[maybe_unused]]  const float& radius) -> MeshData {
   std::vector<simd::float4> position;
   std::vector<simd::float3> normals;
   std::vector<simd::float2> uvs;
   std::vector<std::uint32_t> idx;

   constexpr float PI = std::numbers::pi_v<float>;
   constexpr std::uint32_t numLatitudeLines = 100;

   for (std::uint32_t lat = 0; lat <= numLatitudeLines; ++lat) {
      constexpr std::uint32_t numLongLines = 100;
      const float theta = static_cast<float>(lat) * PI / numLatitudeLines;
      const float sinTheta = simd::sin(theta);
      const float cosTheta = simd::cos(theta);

      for (std::uint32_t lon = 0; lon <= numLongLines; ++lon) {
         const float phi = static_cast<float>(lon) * 2.0f * PI / numLongLines;
         const float sinPhi = simd::sin(phi);
         const float cosPhi = simd::cos(phi);

         const auto pos = simd::float4{cosPhi * sinTheta, cosTheta, sinPhi * sinTheta,1.0};
         position.emplace_back(pos);
         normals.emplace_back(simd::normalize(pos.xyz));
         uvs.emplace_back(simd::float2{
            static_cast<float>(lon)/numLongLines,
            1.0f - static_cast<float>(lat)/numLatitudeLines
         });

         const std::uint32_t first = lat * (numLongLines + 1) + lon;
         const std::uint32_t second = (lat + 1) * (numLongLines + 1) + lon;

         idx.emplace_back(first);
         idx.emplace_back(second);
         idx.emplace_back(first + 1);
         idx.emplace_back(second);
         idx.emplace_back(second + 1);
         idx.emplace_back(first + 1);
      }
   }
   return MeshData{createModelData(position,normals, normals,normals,uvs),std::move(idx)};
}

}
