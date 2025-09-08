#ifndef GAME_TUTORIAL_VECTOR3_HPP
#define GAME_TUTORIAL_VECTOR3_HPP

#include "error.hpp"
#include <simd/vector.h>

namespace game {

class Vector3 {
public:
   Vector3(const float x, const float y, const float z) : _vector{x,y,z}{}
   explicit Vector3(const simd::float3& vec) : _vector(vec.xyz) {}
   explicit Vector3(const simd::float4& vec) : _vector(vec.xyz) {}
   [[nodiscard]] auto x() const -> float {return _vector.x;}
   [[nodiscard]] auto y() const -> float {return _vector.y;}
   [[nodiscard]] auto z() const -> float {return _vector.z;}

   [[nodiscard]] constexpr auto data() const -> const simd::float3& {return _vector;}
   [[nodiscard]] auto norm() const -> float {return simd::length(_vector);}
   [[nodiscard]] auto normalize() const -> Vector3 {return static_cast<Vector3>(simd::normalize(_vector));}

   auto operator[](const size_t& index) const -> float {
      ensure(index < 3, std::format("Invalid index {}!",index));
      return _vector[index];
   }

   auto operator->() const {
      return &_vector;
   }
private:
   simd::float3 _vector;
};

}

template<>
struct std::formatter<game::Vector3> {
   static constexpr auto parse(const std::format_parse_context &ctx) {
      return std::cbegin(ctx);
   }

   static auto format(const game::Vector3 &vec, std::format_context &ctx) {
      return std::format_to(ctx.out(),
         "{0} {1} {2}",
         vec.x(),vec.y(),vec.z()
         );
   }
};

#endif //GAME_TUTORIAL_VECTOR3_HPP