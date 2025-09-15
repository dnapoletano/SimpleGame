#ifndef GAME_TUTORIAL_CAMERA_HPP
#define GAME_TUTORIAL_CAMERA_HPP

#include "matrix4.hpp"
#include "vector3.hpp"

namespace game {
class Camera {
public:
   Camera(const float fov, const float width,
      const float height, const float near_plane,
      const float far_plane,
      const Vector3& eye, const Vector3& look_at, const Vector3& up)
      : _fov(fov), _width(width), _height(height), _nearPlane(near_plane), _farPlane{far_plane},
      _eye(eye), _lookAt(look_at), _up(up),
      _camera(Matrix4::perspective(_fov, _width, _height, _nearPlane, _farPlane) * Matrix4::lookAt(_eye, _lookAt, _up)) {}

   [[nodiscard]] constexpr auto getData() const -> const simd::float4x4 * { return &(_camera.data());}
   [[nodiscard]] constexpr auto size() const -> size_t { return sizeof(_camera.data());}
   [[nodiscard]] constexpr auto getCamera() const -> const Matrix4& {return _camera;}
   auto translate(const Vector3& translate) -> void {
      _eye = Matrix4(translate)*_eye;
      _lookAt = Matrix4(translate) * _lookAt;
      _camera = Matrix4::perspective(_fov, _width, _height, _nearPlane, _farPlane) * Matrix4::lookAt(_eye, _lookAt, _up);
   }

private:
   float _fov{0.0f}, _width{0.0f}, _height{0.0f}, _nearPlane{0.0f}, _farPlane{0.0f};
   Vector3 _eye, _lookAt, _up;
   Matrix4 _camera;
};
}
template<>
struct std::formatter<game::Camera> {
   static constexpr auto parse(const std::format_parse_context &ctx) {
      return std::cbegin(ctx);
   }

   static auto format(const game::Camera &camera, std::format_context &ctx) {
      const auto matrix_data = camera.getCamera().data();
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

#endif // GAME_TUTORIAL_CAMERA_HPP
