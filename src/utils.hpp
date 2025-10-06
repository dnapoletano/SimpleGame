#ifndef GAME_TUTORIAL_UTILS_HPP
#define GAME_TUTORIAL_UTILS_HPP

#include <ranges>
#include <utility>

template <std::ranges::viewable_range R>
auto enumerate(R&& r) {
   auto indices = std::views::iota(size_t{0});
   return std::views::zip(std::move(indices), std::forward<R>(r));
}


#endif // GAME_TUTORIAL_UTILS_HPP
