#ifndef GAME_TUTORIAL_MATRIX4_HPP
#define GAME_TUTORIAL_MATRIX4_HPP

#include <span>
#include <iomanip>
#include <format>
#include <simd/simd.h>
#include <simd/math.h>
#include <simd/matrix.h>
#include <simd/matrix_types.h>

#include "vector3.hpp"

namespace game {
class Matrix4 {
public:
   Matrix4();
   explicit Matrix4(const simd::float4x4& inmatrix) : _elements(inmatrix) {}
   explicit Matrix4(const Vector3& translation);
   explicit Matrix4(Vector3 axis, const float& theta);
   [[nodiscard]] constexpr auto data() const -> const simd::float4x4& {return _elements;}
   auto operator->()  {
      return &_elements;
   }

   constexpr auto operator*(const Vector3& vec) const -> Vector3 {
      return (_elements * simd::float4{vec.x(),vec.y(),vec.z(),1.f});
   }

   constexpr auto operator*(const Matrix4& matrix) const -> Matrix4 {
      return Matrix4(_elements * matrix.data());
   }


   static constexpr auto lookAt(const Vector3& eye, const Vector3& look_at, const Vector3& up) -> Matrix4 {
      const auto f = simd::normalize((eye-look_at).data());
      const auto x = simd::normalize(simd::cross(up.data(),f));
      const auto y = simd::cross(f,x);
      const auto t = -simd::make_float3((eye*x), (eye*y), (eye*f));

      simd::float4x4 viewMatrix;
      viewMatrix.columns[0] = simd::make_float4(x.x,y.x,f.x, 0.0f);
      viewMatrix.columns[1] = simd::make_float4(x.y,y.y,f.y, 0.0f);
      viewMatrix.columns[2] = simd::make_float4(x.z,y.z,f.z, 0.0f);
      viewMatrix.columns[3] = simd::make_float4(t, 1.0f);

      return Matrix4(viewMatrix);
   }

   static constexpr auto perspective(const float fov, const float width,
      const float height, const float near_plane,
      const float far_plane) -> Matrix4 {

      Matrix4 m;
      const auto aspect_ratio = width/height;
      const auto t = simd::tan(fov/2.0f) * near_plane;
      const auto b = -t;
      const auto r = t * aspect_ratio;
      const auto l = b * aspect_ratio;

      m->columns[0] = simd::make_float4(2.f*near_plane/(r-l) , 0.0f,                 0.0f,                                              0.0f);
      m->columns[1] = simd::make_float4(0.0f,                  2.f*near_plane/(t-b), 0.0f,                                              0.0f);
      m->columns[2] = simd::make_float4((r+l)/(r-l),           (t+b)/(t-b),          (far_plane + near_plane)/(near_plane - far_plane), -1.0f);
      m->columns[3] = simd::make_float4(0.0f,                  0.0f,                 2.f*far_plane*near_plane/(near_plane-far_plane), 0.0f);

      return m;

   }

   static constexpr auto orthografic(const float left, const float right,
      const float bottom, const float top, const float near, const float far) -> Matrix4 {
      Matrix4 m;
      m->columns[0] = simd::make_float4(2.0f/(right-left),0.0f,0.0f,0.0f);
      m->columns[1] = simd::make_float4(0.0f,2.0f/(top-bottom),0.0f,0.0f);
      m->columns[2] = simd::make_float4(0.0f,0.0f,1.0f/(far-near),0.0f);
      m->columns[3] = simd::make_float4((left+right)/(left-right),(top+bottom)/(bottom-top),near/(near-far),1.0f);
      return m;
   }

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
         matrix_data.columns[1][0],
         matrix_data.columns[2][0],
         matrix_data.columns[3][0],
         matrix_data.columns[0][1],
         matrix_data.columns[1][1],
         matrix_data.columns[2][1],
         matrix_data.columns[3][1],
         matrix_data.columns[0][2],
         matrix_data.columns[1][2],
         matrix_data.columns[2][2],
         matrix_data.columns[3][2],
         matrix_data.columns[0][3],
         matrix_data.columns[1][3],
         matrix_data.columns[2][3],
         matrix_data.columns[3][3]
         );
   }
};

#endif //GAME_TUTORIAL_MATRIX4_HPP