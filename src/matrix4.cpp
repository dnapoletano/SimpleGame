#include "matrix4.hpp"
#include "vector3.hpp"

namespace game {
Matrix4::Matrix4()
   : _elements{matrix_identity_float4x4}{
}

Matrix4::Matrix4(const Vector3& translation)
   : Matrix4() {
   _elements.columns[3][0] = translation.x();
   _elements.columns[3][1] = translation.y();
   _elements.columns[3][2] = translation.z();
}

Matrix4::Matrix4(Vector3 axis, const float& theta)
      : Matrix4() {
   if (axis.norm()!=1.f) {
      axis = axis.normalize();
   }
   _elements.columns[0][0] = axis->x*axis->x*(1.f-simd::cos(theta)) + simd::cos(theta);
   _elements.columns[0][1] = axis->x*axis->y*(1.f-simd::cos(theta)) - axis->z*simd::sin(theta);
   _elements.columns[0][2] = axis->x*axis->z*(1.f-simd::cos(theta)) + axis->y*simd::sin(theta);

   _elements.columns[1][0] = axis->y*axis->x*(1.f-simd::cos(theta)) + axis->z*simd::sin(theta);
   _elements.columns[1][1] = axis->y*axis->y*(1.f-simd::cos(theta)) + simd::cos(theta);
   _elements.columns[1][2] = axis->y*axis->z*(1.f-simd::cos(theta)) - axis->x*simd::sin(theta);

   _elements.columns[2][0] = axis->z*axis->x*(1.f-simd::cos(theta)) - axis->y*simd::sin(theta);
   _elements.columns[2][1] = axis->z*axis->y*(1.f-simd::cos(theta)) + axis->x*simd::sin(theta);
   _elements.columns[2][2] = axis->z*axis->z*(1.f-simd::cos(theta)) + simd::cos(theta);
}

auto Matrix4::operator*(const Vector3& vec) const -> Vector3 {
   return static_cast<Vector3>(_elements * simd::float4{vec->x,vec->y,vec->z,1.f});
}

}