#include <metal_stdlib>
using namespace metal;

struct VertexData {
   float4 position;
   float3 normal;
   float2 uv;
};

struct AmbientLight {
   float strenght;
   float4 colour;
};

struct DirectionalLight {
   float strenght;
   float4 colour;
   float3 direction;
};

struct PointLight {
   float strenght;
   float4 colour;
   float3 position;
};

struct VertexPayload {              //Mesh Vertex Type
    float4 position [[position]];   //Qualified attribute
    float3 normal;
    float2 uv;                      // UV coordinates/*
    float4 wPosition;
};
/*
    The vertex qualifier registers this function in the vertex stage of the Metal API.

    Currently we're just taking the Vertex ID, it'll be reset at the start of the draw call
    and increment for each successive invocation.

    See page 99 of the metal spec,
    table 5.2: Attributes for vertex function input arguments,
    for more info.
*/
VertexPayload vertex vertexMain(uint vertexID [[vertex_id]],
    constant VertexData* vertexData,
    constant float4x4& transform [[buffer(1)]],
    constant float4x4& proj [[buffer(2)]]) {

    VertexPayload payload;
    VertexData vert = vertexData[vertexID];
    payload.position = proj*(transform*vert.position);
    payload.wPosition = vert.position;
    payload.normal = vert.normal;
    payload.uv = vert.uv;
    return payload;
}


float4 calcAmbientLight(constant AmbientLight& al, float4 albedo) {
    return float4(al.strenght * al.colour.rgb * albedo.rgb,1.0f);
}

float4 calcDirectionalLight(constant DirectionalLight& dl, float3 normal, float4 albedo) {
    const auto norm = simd::normalize(-dl.direction);
    const auto diff = max(dot(normal,norm),0.0);
    const auto dir_colour = float4((diff * dl.colour).xyz,1.0);
    return float4(dl.strenght * dir_colour.rgb * albedo.rgb,1.0f);
}

float4 calcPointLight(constant PointLight& pl, float3 fragPosition, float3 normal, float4 albedo){
    const auto direction = simd::normalize(pl.position - fragPosition);
    const auto diffuse = max(dot(normal,direction),0.0f);
    const auto distance = length(pl.position - fragPosition);
    return float4(pl.strenght * albedo.rgb * (diffuse / distance/distance) * pl.colour.rgb,1.0);
}
/*
    The vertex qualifier registers this function in the vertex stage of the Metal API.

    See page 104 of the metal spec,
    table 5.5: Attributes for fragment function input arguments,
    for more info.
*/
fragment float4 fragmentMain(VertexPayload frag           [[stage_in]],
                            texture2d<float> colorTexture [[texture(0)]],
                            constant AmbientLight& al     [[buffer(1)]],
                            constant DirectionalLight& dl [[buffer(2)]],
                            constant PointLight& pl       [[buffer(3)]])
{
    constexpr sampler textureSampler (mag_filter::linear,min_filter::linear);
    const auto colorSample   = colorTexture.sample(textureSampler, frag.uv);
    const auto ambient_light = calcAmbientLight(al, colorSample);

    const auto dir_light   = calcDirectionalLight(dl, frag.normal, colorSample);
    const auto point_light = calcPointLight(pl, frag.wPosition.xyz, frag.normal, colorSample);

    // Sample the texture to obtain a color
    return (ambient_light + dir_light + point_light);
}