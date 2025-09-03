#ifndef GAME_TUTORIAL_AUTO_RELEASE_HPP
#define GAME_TUTORIAL_AUTO_RELEASE_HPP

/// Utility for managing resources (similar to smart pointers)

#include <functional>
#include <ranges>

namespace game {
template<class T, T Invalid={}>
class AutoRelease {
public:
   AutoRelease()
      : AutoRelease(Invalid,nullptr) {}

   AutoRelease(T obj, std::function<void(T)> deleter)
      : _obj(obj), _deleter(deleter) {}
   ~AutoRelease() {
      if ((_obj != Invalid) and _deleter) {
         _deleter(_obj);
      }
   }
   AutoRelease(const AutoRelease& ) = delete;
   auto operator=(const AutoRelease&) -> AutoRelease& = delete;

   AutoRelease(AutoRelease&& other) noexcept
      : AutoRelease()
   {
      swap(other);
   }
   auto operator=(AutoRelease&& other) noexcept -> AutoRelease& {
      AutoRelease new_obj{std::move(other)};
      swap(new_obj);
      return *this;
   }

   auto swap(AutoRelease& other) noexcept -> void {
      std::ranges::swap(_obj,other._obj);
      std::ranges::swap(_deleter,other._deleter);
   }

   T get() const {
      return _obj;
   }

   explicit operator T() const {
      return _obj;
   }

   explicit operator bool() const {
      return _obj!=Invalid;
   }
private:
   T _obj;
   std::function<void(T)> _deleter;
};
}

#endif //GAME_TUTORIAL_AUTO_RELEASE_HPP