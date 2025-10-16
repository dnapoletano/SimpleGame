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

vertex float4 shadowVertexFunction(uint vertexID [[vertex_id]], constant VertexData *vertexData,
                                   constant float4x4 &transform [[buffer(1)]], constant float4x4 &viewProj [[buffer(2)]]) {

  float3 wpos = (transform * vertexData[vertexID].position).xyz;
  float4 position = viewProj * float4(wpos,1.0f);
   return position;
}

fragment void shadowFragmentFunction() {}
