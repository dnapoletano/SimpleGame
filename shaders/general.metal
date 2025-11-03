#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;


struct VertexData {
  float4 position;
  float3 normal;
  float3 tangent;
  float3 bitangent;
  float3 color;
  float2 uv;
};

struct VertexPayload {              //Mesh Vertex Type
  float4 position [[position]];   //Qualified attribute
  float3 normal;
  float3 tangent;
  float3 bitangent;
  float3 color;
  float2 uv;                      // UV coordinates/*
  float4 wPosition;
};

struct FragmentPayLoad {
  float4 positions [[color(0)]];
  float4 normals [[color(1)]];
  float4 albedo [[color(2)]];
  float4 specular [[color(3)]];
  float4 roughness [[color(4)]];
};

/*
    The vertex qualifier registers this function in the vertex stage of the Metal API.

    Currently we're just taking the Vertex ID, it'll be reset at the start of the draw call
    and increment for each successive invocation.

    See page 99 of the metal spec,
    table 5.2: Attributes for vertex function input arguments,
    for more info.
*/
constant constexpr float M_PI = 3.14;

VertexPayload vertex vertexMain(uint vertexID [[vertex_id]],
    constant VertexData* vertexData,
    constant float4x4& transform [[buffer(1)]],
    constant float4x4& proj [[buffer(2)]]) {

    VertexPayload payload;
    VertexData vert = vertexData[vertexID];
    payload.position = proj * (transform * vert.position);
    payload.wPosition = transform*vert.position;
    /// these should be transformed
    payload.normal = vert.normal;
    payload.tangent = vert.tangent;
    payload.bitangent = vert.bitangent;
    payload.color = vert.color;
    payload.uv = vert.uv;
    return payload;
}

/*
    The vertex qualifier registers this function in the vertex stage of the Metal API.

    See page 104 of the metal spec,
    table 5.5: Attributes for fragment function input arguments,
    for more info.
*/
fragment FragmentPayLoad fragmentMain(VertexPayload frag [[stage_in]]) {
  FragmentPayLoad out;
  out.positions = float4(frag.wPosition.xyz,1.0f);
  out.normals = float4(normalize(frag.normal) * 0.5 + 0.5, 1.0f);
  out.albedo = float4(frag.color,1.0f);
  out.specular = float4(0.0f);
  out.roughness = float4(0.5f);
  return out;
}
