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

#include <simd/simd.h>

#include <filesystem>

struct Vertex {
   simd::float4 position;
   simd::half3 color;
};

// constant float4 positions[] = {
//    float4(-0.75, -0.75, 1, 1.0), //bottom left: red
//    float4( -0.75, 0.75, 1, 1.0), //bottom right: green
//    float4(  0.75,  0.75, 1, 1.0), //center top: blue
//    float4(-0.75, -0.75, 1, 1.0), //bottom left: red
//    float4( 0.75, 0.75, 1, 1.0), //bottom right: green
//    float4(  0.75,  -0.75, 1, 1.0), //center top: blue
// };
//
// constant half3 colors[] = {
//    half3(1.0, 0.0, 0.0), //bottom left: red
//    half3(0.0, 1.0, 0.0), //bottom right: green
//    half3(0.0, 0.0, 1.0), //center top: blue
//    half3(1.0, 0.0, 0.0), //bottom left: red
//    half3(0.0, 0.0, 1.0), //center top: blue
//    half3(0.0, 1.0, 0.0), //bottom right: green
// };


static constexpr Vertex VerteData[] = {
   {
      .position = {-0.75, -0.75, 1, 1.0},
      .color = {1.0, 0.0, 0.0} },
   {.position = { -0.75, 0.75, 1, 1.0},
      .color = {0.0, 1.0, 0.0}},
   {.position = { 0.75,  0.75, 1, 1.0},
      .color = {0.0, 0.0, 1.0}},
   {.position = {-0.75, -0.75, 1, 1.0},
      .color = {1.0, 0.0, 0.0}},
   {.position = {0.75, 0.75, 1, 1.0},
      .color = {0.0, 0.0, 1.0}},
   {.position = {0.75,  -0.75, 1, 1.0},
      .color = {0.0, 1.0, 0.0}}
};

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
               (error!=nullptr) ? error->localizedDescription()->utf8String():"")
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

      const auto VertexBuffer = game::AutoRelease<MTL::Buffer*,{}>{
         win.getDevice()->newBuffer(&VerteData,sizeof(VerteData),MTL::ResourceStorageModeShared),
         [](auto t) {t->release();}
         };
      game::ensure(VertexBuffer.get()!=nullptr,"Cannot allocate vertex buffer");
      win.setMeshBufer(VertexBuffer.get());
      while (win.running()) {
         win.update();
      }
   } catch (const game::Exception& exception) {
      std::println(std::cerr << game::COLOR_VIOLET,"{}",exception);
   } catch (const std::runtime_error& err) {
      game::Exception exception(err.what());
      std::println(std::cerr << game::COLOR_VIOLET,"{}",exception);
   } catch (...) {
      std::println(std::cerr,"Unkown exception 🥲");
   }
   return 0;
}