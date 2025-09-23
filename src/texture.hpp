#ifndef GAME_TUTORIAL_TEXTURE_HPP
#define GAME_TUTORIAL_TEXTURE_HPP

#include <Metal/Metal.hpp>

#include <cstddef>
#include <span>

#include "auto_release.hpp"

namespace game {

class Texture {
public:
   Texture(const std::vector<std::vector<std::byte>>& datavec, size_t width,
      size_t height, MTL::Device * device);

   [[nodiscard]] auto getTexture() const -> MTL::Texture* {return _texture.get();}

private:
   AutoRelease<MTL::Texture*,{}> _texture;
};

}

#endif // GAME_TUTORIAL_TEXTURE_HPP
