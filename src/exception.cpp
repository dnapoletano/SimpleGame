#include <format>
#include "exception.hpp"

namespace game {
   Exception::Exception(const std::string& what)
      : std::runtime_error(what),
        _trace(std::stacktrace::current(1))
   {

   }

   auto Exception::stackTrace() const -> std::string {
      return std::to_string(_trace);
   }

}
