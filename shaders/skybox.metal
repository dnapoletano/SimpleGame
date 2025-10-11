#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

struct VertexData {
   float4 position;
   float3 normal;
   float3 tangent;
   float3 bitangent;
   float2 uv;
};

struct VertexPayload {
   float4 position [[position]];
  float4 wpos;
};


VertexPayload vertex vertexMain(uint vertexID [[vertex_id]], constant VertexData *vertexData,
                                constant float4x4 &transform [[buffer(1)]], constant float4x4 &camera [[buffer(2)]],
                                constant float4x4 &proj [[buffer(3)]]) {

   VertexPayload vout;
   auto view = camera;
   view.columns[3] = float4(0.0f,0.0f,0.0f,1.0f);
   vout.position = (proj * view * (vertexData[vertexID].position));
   vout.position.z = vout.position.w;
   vout.wpos = vertexData[vertexID].position;

    return vout;
}

fragment float4 fragmentMain(VertexPayload frag [[stage_in]], texturecube<float> colorCube [[texture(0)]]) {
  constexpr sampler textureSampler (address::clamp_to_edge,filter::linear);
  return colorCube.sample(textureSampler, normalize(frag.wpos.xyz));
}
