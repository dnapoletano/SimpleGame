#ifndef GAME_TUTORIAL_MATERIAL_HPP
#define GAME_TUTORIAL_MATERIAL_HPP

#include <fstream>
#include <map>
#include <string>
#include <Metal/Metal.hpp>

#include "auto_release.hpp"
#include "error.hpp"

namespace game {
typedef std::map<std::string,MTL::Function *> ShaderFunctions;

class Material {
public:
   Material(const std::string_view shader, MTL::Device * const device)
      : _device(device){

      const auto shader_source = NS::String::string(shader.data(),NS::ASCIIStringEncoding);
      NS::Error * error = nullptr;
      MTL::Library * defaultLibrary = _device->newLibrary(shader_source, {}, &error);
      game::ensure(defaultLibrary != nullptr,
         std::format("no library ->\n{}",
                  (error!=nullptr) ? error->localizedDescription()->utf8String():"")
         );

      auto str = NS::String::string("vertexMain",NS::ASCIIStringEncoding);
      MTL::Function *vertexFunction = defaultLibrary->newFunction(str);
      str = NS::String::string("fragmentMain",NS::ASCIIStringEncoding);
      MTL::Function *fragmentFunction = defaultLibrary->newFunction(str);
      defaultLibrary->release();
      _shaderFunctions["vertexShader"]   = vertexFunction;
      _shaderFunctions["fragmentShader"] = fragmentFunction;
   }

   [[nodiscard]] auto getShaderFunctions() const -> ShaderFunctions {return _shaderFunctions;}

   constexpr auto setUpRenderPipeLineState(const CA::MetalLayer* const layer) -> void {
      /// if the pipeline state is already set, do not do anything
      /// and throw an error, as this should only happen once
      /// per material.
      ensure(static_cast<bool>(_rps) == false,
         "Render pipeline state is already set, you shouldn't be here");

      const auto pipeline_descriptor = AutoRelease<MTL::RenderPipelineDescriptor *,{}>{
         MTL::RenderPipelineDescriptor::alloc()->init(),
         [](auto t) {t->release();}};

      const auto attach = pipeline_descriptor->colorAttachments()->object(0);

      pipeline_descriptor->setVertexFunction(_shaderFunctions["vertexShader"]);
      pipeline_descriptor->setFragmentFunction(_shaderFunctions["fragmentShader"]);
      pipeline_descriptor->setSampleCount(4);
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
      const auto depth_stencil_descriptor = AutoRelease<MTL::DepthStencilDescriptor*,{}>{
         MTL::DepthStencilDescriptor::alloc()->init(),
         [](auto t) {t->release();}
      };
      depth_stencil_descriptor->setDepthCompareFunction(MTL::CompareFunctionLess);
      depth_stencil_descriptor->setDepthWriteEnabled(true);
      _dss = {
         _device->newDepthStencilState(depth_stencil_descriptor.get()),
         [](auto t) {t->release();}
      };
   }

   [[nodiscard]] constexpr auto getRenderPipelineState() const-> MTL::RenderPipelineState* {return _rps.get();}
   [[nodiscard]] constexpr auto getDepthStencilState() const -> MTL::DepthStencilState* {return _dss.get();}

private:
   ShaderFunctions _shaderFunctions;
   MTL::Device * const _device;
   AutoRelease<MTL::RenderPipelineState*,{}>      _rps{};
   AutoRelease<MTL::DepthStencilState*,{}>        _dss{};
};
}

#endif // GAME_TUTORIAL_MATERIAL_HPP
