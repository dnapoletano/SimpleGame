#ifndef GAME_TUTORIAL_RESOURCE_READER_HPP
#define GAME_TUTORIAL_RESOURCE_READER_HPP

#include "error.hpp"

#include <filesystem>
#include <fstream>
#include <ranges>
#include <string_view>
#include <sstream>

namespace game {

class ResourceLoader {
public:
   ResourceLoader(const std::filesystem::path &root) : _root(root) {}

   [[nodiscard]] auto loadString(const std::string_view path) const -> std::string {
      return loadFile(_root / path, std::ios::in);
   }

   [[nodiscard]] auto loadBytes(const std::string_view path) const -> std::vector<std::byte> {
      const auto data =  loadFile(_root / path, std::ios::in | std::ios::binary);
      return data | std::views::transform([](auto b) -> std::byte{return static_cast<std::byte>(b);}) | std::ranges::to<std::vector>();
   }

   auto static loadFile(const std::filesystem::path &path, auto open_mode) -> std::string {
      const std::ifstream file{path,open_mode};
      ensure(file.is_open(),
         "Could not open file");

      std::stringstream strm {};
      strm << file.rdbuf();
      return strm.str();
   }

private:
   std::filesystem::path _root;
};

}


#endif // GAME_TUTORIAL_RESOURCE_READER_HPP
