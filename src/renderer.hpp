#ifndef GAME_TUTORIAL_RENDERER_HPP
#define GAME_TUTORIAL_RENDERER_HPP

#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>

#include "auto_release.hpp"
#include "camera.hpp"
#include "entity.hpp"
#include "material.hpp"

namespace game {

class Renderer {
public:
   Renderer(MTL::Device* device, CA::MetalLayer* layer);

/// TODO: check as this is now passed by copy, as maps do not have
///       const access member functions
   auto setUpPipelineState(ShaderFunctions funcs) -> void;
   auto render(const Camera& camera) const -> void;

   auto setMeshBufer(const Entity& entity) -> void;
   [[nodiscard]] auto getMeshBuffer() const -> MTL::Buffer * {return _mesh_buffer;}

private:
   AutoRelease<MTL::RenderPipelineState*,{}>      _rps{};
   AutoRelease<MTL::DepthStencilState*,{}>        _dss{};
   MTL::Buffer* _mesh_buffer{nullptr};

   MTL::Device*    _device{nullptr};
   CA::MetalLayer* _layer{nullptr};
};

}

#endif // GAME_TUTORIAL_RENDERER_HPP
