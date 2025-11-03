#ifndef GAME_TUTORIAL_RENDERER_HPP
#define GAME_TUTORIAL_RENDERER_HPP

#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>

#include "auto_release.hpp"
#include "camera.hpp"
#include "scene.hpp"

namespace game {

class Renderer {
public:
   Renderer(MTL::Device* device);

   auto createDepthTexture(const std::uint32_t width, const std::uint32_t height) -> void ;
   auto createRenderPassesTexture(const std::uint32_t width, const std::uint32_t height) -> void ;
   auto geometryPass(const Camera& camera,
      CA::MetalDrawable* surface, Scene& scene) const -> void;
   auto lightingPipelineState() -> void;
   auto render(const Camera& camera, const CA::MetalDrawable* surface, Scene& scene) const -> void;


private:
   MTL::Device*    _device{nullptr};
   AutoRelease<MTL::Texture*> _depthTexture{};
   std::vector<AutoRelease<MTL::Texture*>> _renderPassTextures;
   AutoRelease<MTL::Texture*> _outputTexture{};
   AutoRelease<MTL::DepthStencilState*,{}>        _shadowdss{};
   AutoRelease<MTL::ComputePipelineState*,{}>     _lightingcps{};
};

}

#endif // GAME_TUTORIAL_RENDERER_HPP
