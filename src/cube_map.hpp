#ifndef GAME_TUTORIAL_CUBE_MAP_HPP
#define GAME_TUTORIAL_CUBE_MAP_HPP

#include <cstddef>
#include <vector>
#include <span>
#include <string_view>
#include <map>

#include "auto_release.hpp"
#include "mesh.hpp"
#include "mesh_factory.hpp"

#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>

#include "matrix4.hpp"


namespace game {
typedef std::map<std::string,MTL::Function *> ShaderFunctions;

class CubeMap {

public:
   CubeMap(const std::vector<std::span<const std::byte>>& faces,
      std::string_view shader,
      std::uint32_t width, std::uint32_t height,MTL::Device * device, MeshFactory * mf);

   [[nodiscard]] auto getTextures() const -> MTL::Texture* {return _texture.get();}

   [[nodiscard]] auto getShaderFunctions() const -> ShaderFunctions {return _shaderFunctions;}

   constexpr auto setUpRenderPipeLineState(const CA::MetalLayer* layer) -> void {
      {
         ensure(static_cast<bool>(_rps) == false,
            "Render pipeline state is already set, you shouldn't be here");

         const auto pipeline_descriptor = AutoRelease<MTL::RenderPipelineDescriptor *,{}>{
            MTL::RenderPipelineDescriptor::alloc()->init(),
            [](auto t) {t->release();}};

         const auto attach = pipeline_descriptor->colorAttachments()->object(0);

         pipeline_descriptor->setVertexFunction(_shaderFunctions["vertexShader"]);
         pipeline_descriptor->setFragmentFunction(_shaderFunctions["fragmentShader"]);
         pipeline_descriptor->setSampleCount(1);
         pipeline_descriptor->setDepthAttachmentPixelFormat(MTL::PixelFormatDepth32Float);
         attach->setPixelFormat(layer->pixelFormat());
         NS::Error * error = nullptr;
         _rps = {
            _device->newRenderPipelineState(pipeline_descriptor.get(),&error),
            [](auto t){t->release();}
         };

         game::ensure(_rps.get() != nullptr,
            std::format("non pipeline descriptor ->\n{}",
                     (error!=nullptr)?error->description()->utf8String():"")
            );
      }
   }

   [[nodiscard]] constexpr auto getRenderPipelineState() const-> MTL::RenderPipelineState* {return _rps.get();}
   constexpr auto createBuffers(MTL::Device * device) const-> void {_cubeMesh->createBuffers(device);}

   [[nodiscard]] constexpr auto getVertexData() const -> const VertexData*  { return _cubeMesh->getVertexArray().data();}
   [[nodiscard]] constexpr auto getVertexBuffer() const -> MTL::Buffer*  { return _cubeMesh->getVertexBuffer();}
   [[nodiscard]] constexpr auto getIndexBuffer() const -> MTL::Buffer*  { return _cubeMesh->getIndexBuffer();}
   [[nodiscard]] constexpr auto getIndexCount() const -> size_t  { return _cubeMesh->getIndexCount();}
   [[nodiscard]] constexpr auto size() const -> size_t { return _cubeMesh->size();}
   [[nodiscard]] constexpr auto getPrimitive() const -> MTL::PrimitiveType {return _cubeMesh->getPrimitiveType();}
   [[nodiscard]] constexpr auto getVertexCount() const -> size_t {return _cubeMesh->n_verts();}
   [[nodiscard]] constexpr auto getModel() const -> Matrix4 {return {};}

private:
   AutoRelease<MTL::Texture*,{}> _texture;
   ShaderFunctions _shaderFunctions;
   MTL::Device * const _device;
   AutoRelease<MTL::RenderPipelineState*,{}>      _rps{};
   AutoRelease<game::Mesh *> _cubeMesh{};
};
}

#endif // GAME_TUTORIAL_CUBE_MAP_HPP
