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
#include "entity.hpp"

#include <filesystem>

auto main() -> int {
   try {
      game::Window win(600u,400u);
      game::Mesh mesh{};
      game::Material material{"../../shaders/general.metal",win.getDevice()};
      const game::Entity cube{&mesh,&material};
      const auto renderer = win.getRenderer();
      renderer->setUpPipelineState(cube.getShaderFunctions());
      renderer->setMeshBufer(cube);

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