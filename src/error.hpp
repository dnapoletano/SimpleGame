#ifndef GAME_TUTORIAL_ERROR_H
#define GAME_TUTORIAL_ERROR_H

#include <format>
#include <string_view>
#include "exception.hpp"

namespace game {
   static constexpr std::string COLOR_VIOLET ="\x1b[48;5;105m";
   static  constexpr std::string RESET_COLOR = "\x1b[0m";

   template <class... Args>
   inline auto ensure(const bool predicate,
      const std::string_view& msg,
      Args && ...args) -> void {
      if (!predicate) {
         const std::string message = std::format("{}",msg);
         throw Exception(std::vformat(msg,
            std::make_format_args(std::forward<Args>(args)...)));
      }
   }
}

#endif //GAME_TUTORIAL_ERROR_H