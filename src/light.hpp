#ifndef GAME_TUTORIAL_LIGHT_HPP
#define GAME_TUTORIAL_LIGHT_HPP


namespace game {

struct AmbientLight {
   float strenght;
   simd::float4 colour;
};

struct DirectionalLight {
   float strenght;
   simd::float4 colour;
   simd::float3 direction;
};

struct PointLight {
   float strenght;
   simd::float4 colour;
   simd::float3 position;
};

}
#endif // GAME_TUTORIAL_LIGHT_HPP
