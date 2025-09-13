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
   Material(const std::string& filename, MTL::Device * const device) {
      std::ifstream shader(filename,std::ios::in);
      game::ensure(shader.is_open(), std::format("Couldn't open shader file -> {}",filename));

      const std::string shader_string(
         std::istreambuf_iterator<char>{shader},
         {});
      const auto shader_source = NS::String::string(shader_string.data(),NS::ASCIIStringEncoding);
      NS::Error * error = nullptr;
      MTL::Library * defaultLibrary = device->newLibrary(shader_source, {}, &error);
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
private:
   ShaderFunctions _shaderFunctions;
};
}

#endif // GAME_TUTORIAL_MATERIAL_HPP
