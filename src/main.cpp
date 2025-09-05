#include <iostream>
#include <fstream>
#include <print>
#include <format>

#define NS_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION

#include "error.hpp"
#include "exception.hpp"
#include "window.hpp"

#include <filesystem>

auto initShader(const std::string& filename, MTL::Device * const device) ->
   game::ShaderFunctions
{
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
               (error!=nullptr) ? error->description()->utf8String():"")
      );

   auto str = NS::String::string("vertexMain",NS::ASCIIStringEncoding);
   MTL::Function *vertexFunction = defaultLibrary->newFunction(str);
   str = NS::String::string("fragmentMain",NS::ASCIIStringEncoding);
   MTL::Function *fragmentFunction = defaultLibrary->newFunction(str);
   defaultLibrary->release();
   return {vertexFunction,fragmentFunction};
}

auto main() -> int {
   try {
      game::Window win(600u,400u);
      win.setUpPipelineState(initShader("../../shaders/triangle.metal", win.getDevice()));

      while (win.running()) {
         win.update();
      }
   } catch (const game::Exception& exception) {
      std::println(std::cerr << game::COLOR_VIOLET,"{}",exception);
   } catch (...) {
      std::println(std::cerr,"Unkown exception 🥲");
   }
   return 0;
}
