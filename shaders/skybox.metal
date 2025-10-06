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
   const auto view = float4x4(camera.columns[0], camera.columns[1], camera.columns[2], float4(0.0f, 0.0f, 0.0f, 1.0f));
   vout.position = proj * view * vertexData[vertexID].position;
   vout.wpos = vertexData[vertexID].position;
   return vout;

}

fragment float4 fragmentMain(VertexPayload frag [[stage_in]], texturecube<float> colorCube [[texture(0)]]) {
    constexpr sampler textureSampler (mag_filter::linear,min_filter::linear);
    //return float4((normalize(frag.wpos.xyz) * 0.5 + 0.5), 1.0);

    return colorCube.sample(textureSampler, normalize(frag.wpos.xyz));
}
