#include <gtest/gtest.h>

#include "matrix4.cpp"
#include "utils.hpp"

#include <print>
#include <vector>

TEST(matrix4, projection) {
   constexpr auto width = 1200u;
   constexpr auto height = 800u;

   const game::Matrix4 proj = game::Matrix4::perspective(
      std::numbers::pi_v<float> / 4.0f,
      width,
      height,
      0.1f,
      100.0f
      );


   const game::Matrix4 view = game::Matrix4::lookAt(
      game::Vector3{0.0f,0.0f,5.0f},
      game::Vector3{0.0f,0.0f,0.0f},
      game::Vector3{0.0f,1.0f,0.0f}
      );


   const auto positions = std::vector<simd::float4>{
      simd::float4{0.0f,0.0f,0.0f,1.0f},
      simd::float4{0.0f,0.0f,4.0f,1.0f},
      simd::float4{1.0f,1.0f,0.0f,1.0f}
   };

   auto print_float4 = [](const simd::float4& p) {
     std::println("({0}, {1}, {2}, {3})",p.x,p.y,p.z,p.w);
   };

   for (const auto &[i,p] : ::enumerate(positions)) {
      print_float4(p);
      const auto pview = view.data()*p;
      print_float4(pview);
      const auto pclip = proj.data()*pview;
      print_float4(pclip);
      std::println("Pos {0} => View : {1}, Clip : {2}", i, game::Vector3{pview}, game::Vector3{pclip/pclip.w});
   }

   ASSERT_TRUE(true);
}
