#include <iostream>
#include <format>
#include <numbers>
#include <print>

#define NS_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION

#include "camera.hpp"
#include "error.hpp"
#include "exception.hpp"
#include "window.hpp"

#include <filesystem>

auto main() -> int {
   try {
      constexpr auto width = 1200u;
      constexpr auto height = 800u;
      game::Window win(width,height);
      game::Scene scene{win.getDevice(),win.getLayer()};


      const game::Camera camera{std::numbers::pi_v<float> / 4.0f,
                          width,height,
                          0.1f,
                          100.0f,
                          {0.0f, 1.0f, 2.0f},
                          {0.0f, 0.0f, 0.0f},
                          {0.0f, 1.0f, 0.0f}};
      scene.setCamera(const_cast<game::Camera*>(&camera));
      while (win.running()) {
         win.update(scene);
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