#ifndef GAME_TUTORIAL_MATRIX4_HPP
#define GAME_TUTORIAL_MATRIX4_HPP

#include <span>
#include <iomanip>
#include <format>
#include <simd/math.h>
#include <simd/matrix.h>
#include <simd/matrix_types.h>

#include "vector3.hpp"

namespace game {
class Matrix4 {
public:
   Matrix4();
   explicit Matrix4(const Vector3& translation);
   explicit Matrix4(Vector3 axis, const float& theta);
   [[nodiscard]] constexpr auto data() const -> const simd::float4x4& {return _elements;}

   auto operator*(const Vector3& vec) const -> Vector3;
private:
   simd::float4x4 _elements;
};
}


template<>
struct std::formatter<game::Matrix4> {
   static constexpr auto parse(const std::format_parse_context &ctx) {
      return std::cbegin(ctx);
   }

   static auto format(const game::Matrix4 &matrix, std::format_context &ctx) {
      const auto matrix_data = matrix.data();
      return std::format_to(ctx.out(),
         "{0} {1} {2} {3}\n{4} {5} {6} {7}\n{8} {9} {10} {11}\n{12} {13} {14} {15}",
         matrix_data.columns[0][0],
         matrix_data.columns[0][1],
         matrix_data.columns[0][2],
         matrix_data.columns[0][3],
         matrix_data.columns[1][0],
         matrix_data.columns[1][1],
         matrix_data.columns[1][2],
         matrix_data.columns[1][3],
         matrix_data.columns[2][0],
         matrix_data.columns[2][1],
         matrix_data.columns[2][2],
         matrix_data.columns[2][3],
         matrix_data.columns[3][0],
         matrix_data.columns[3][1],
         matrix_data.columns[3][2],
         matrix_data.columns[3][3]
         );
   }
};

#endif //GAME_TUTORIAL_MATRIX4_HPP