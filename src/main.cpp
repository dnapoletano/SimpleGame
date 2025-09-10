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
#include "mesh.hpp"
#include "matrix4.hpp"
#include "vector3.hpp"

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
      win.setUpPipelineState(initShader("../../shaders/general.metal", win.getDevice()));
      game::Mesh mesh{};

      const auto VertexBuffer = game::AutoRelease<MTL::Buffer*,{}>{
         win.getDevice()->newBuffer(mesh.getVertexArray().data(),
            mesh.size(),
            MTL::ResourceStorageModeShared),
         [](auto t) {t->release();}
         };
      game::ensure(VertexBuffer.get()!=nullptr,
         "Cannot allocate vertex buffer");

      win.setMeshBufer(VertexBuffer.get());
      //auto translation = game::Matrix4(game::Vector3(0.001f, 0.001f,0.f));


      while (win.running()) {


         // for (std::vector<game::VertexData>::iterator it = mesh.accessVertexArray()->begin(); it!=mesh.accessVertexArray()->end(); ++it) {
         //    auto vertex_position = static_cast<game::Vector3>(it->position);
         //    vertex_position = (rot*vertex_position);
         //    it->position = simd::float4{vertex_position->x,vertex_position->y,vertex_position->z,1.f};
         // }
         // const auto VertexBuffer_l = game::AutoRelease<MTL::Buffer*,{}>{
         //    win.getDevice()->newBuffer(mesh.getVertexArray().data(),
         //       mesh.size(),
         //       MTL::ResourceStorageModeShared),
         //    [](auto t) {t->release();}
         // };
         // game::ensure(VertexBuffer_l.get()!=nullptr,
         //    "Cannot allocate vertex buffer");
         //
         // win.setMeshBufer(VertexBuffer_l.get());


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