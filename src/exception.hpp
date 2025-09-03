#ifndef GAME_TUTORIAL_EXCEPTION_HPP
#define GAME_TUTORIAL_EXCEPTION_HPP

#include <stdexcept>
#include <string>
#include <stacktrace>


namespace game {

class Exception : public std::runtime_error {
public:
   explicit Exception(const std::string& what);
   auto stackTrace() const -> std::string;

private:
   std::stacktrace _trace;
};


}


template<>
struct std::formatter<game::Exception> {
   constexpr auto parse(const std::format_parse_context &ctx) {
      return std::cbegin(ctx);
   }

   auto format(const game::Exception &exception, std::format_context &ctx) const {
      return std::format_to(ctx.out(), "{}\n {}", exception.what(), exception.stackTrace());
   }
};
#endif //GAME_TUTORIAL_EXCEPTION_HPP