#ifndef GAME_TUTORIAL_RENDERER_HPP
#define GAME_TUTORIAL_RENDERER_HPP

#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>

#include "auto_release.hpp"
#include "camera.hpp"
#include "scene.hpp"

namespace game {

enum class RenderPasses {
   MainPass = 0u,
   ShadowPass = 1u
};

class Renderer {
public:
   Renderer(MTL::Device* device);

   auto createDepthTexture(const std::uint32_t width, const std::uint32_t height) -> void ;
   auto createShadowTexture(const std::uint32_t width, const std::uint32_t height) -> void ;
   auto shadowPass(const Camera& camera,
      CA::MetalDrawable* surface, Scene& scene) const -> void;

   auto shadowPipelineState(const CA::MetalLayer* const layer) -> void;

   auto render(const Camera& camera,
      CA::MetalDrawable* surface, Scene& scene) const -> void;


private:
   MTL::Device*    _device{nullptr};
   AutoRelease<MTL::Texture*> _depthTexture{};
   AutoRelease<MTL::Texture*> _shadowPassTexture{};
   AutoRelease<MTL::RenderPipelineState*,{}>      _shadowrps{};
   AutoRelease<MTL::DepthStencilState*,{}>        _shadowdss{};
};

}

#endif // GAME_TUTORIAL_RENDERER_HPP
