#ifndef GAME_TUTORIAL_EXCEPTION_HPP
#define GAME_TUTORIAL_EXCEPTION_HPP

#include <stdexcept>
#include <string>
#include <iomanip>
#include <print>

#include <execinfo.h>


namespace game {

class StackTrace {
public:
   StackTrace(int skip_lines = 0);
   [[nodiscard]] auto getTrace() const-> const std::string& {
      return _trace;
   }

private:
   std::string _trace;
   static constexpr uint16_t  MAX_BACKTRACE_DEPTH = 128;
   static auto _demangle(const std::string& name) -> std::string;
};


class Exception final : public std::runtime_error {
public:
   explicit Exception(const std::string& what);
   [[nodiscard]] auto stackTrace() const -> std::string;

private:
   StackTrace _trace;
};


}


template<>
struct std::formatter<game::Exception> {
   static constexpr auto parse(const std::format_parse_context &ctx) {
      return std::cbegin(ctx);
   }

   static auto format(const game::Exception &exception, std::format_context &ctx) {
      return std::format_to(ctx.out(), "{}\n {}", exception.what(), exception.stackTrace());
   }
};
#endif //GAME_TUTORIAL_EXCEPTION_HPP