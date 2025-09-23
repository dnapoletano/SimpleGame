#include <metal_stdlib>
using namespace metal;

struct VertexData {
    float4 position;
    float3 normal;
    float2 uv;                      // UV coordinates
};

struct VertexPayload {              //Mesh Vertex Type
    float4 position [[position]];   //Qualified attribute
    float3 normal;
    float2 uv;                      // UV coordinates/*
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
    payload.normal = vert.normal;
    payload.uv = vert.uv;
    return payload;
}


float4 calcAmbientLight(float4 albedo) {
    constexpr auto ambient_light_factor = 0.3f;
    constexpr auto ambient_light_color = float4(1.0f,1.0f,1.0f,1.0f);
    const auto ambient_light = ambient_light_factor * ambient_light_color;
    return ambient_light * albedo;
}

float4 calcDirectionalLight(float3 normal, float4 albedo) {
    constexpr auto directional_light_intensity = 0.5f;
    constexpr auto directional_light_color = float4(1.0f,1.0f,1.0f,1.0f);
    const auto directional_light_direction = simd::normalize(float3(1.0f,1.0f,1.0f));
    const auto diff = max(dot(normal,directional_light_direction),0.0);
    const auto dir_colour = float4((diff * directional_light_color).xyz,1.0);
    return dir_colour * albedo;
}
/*
    The vertex qualifier registers this function in the vertex stage of the Metal API.

    See page 104 of the metal spec,
    table 5.5: Attributes for fragment function input arguments,
    for more info.
*/
fragment float4 fragmentMain(VertexPayload frag [[stage_in]],
                            texture2d<float> colorTexture [[ texture(0) ]])
{
    constexpr sampler textureSampler (mag_filter::linear,min_filter::linear);
    const auto colorSample = colorTexture.sample(textureSampler, frag.uv);
    const auto ambient_light = calcAmbientLight(colorSample);

    const auto dir_light = calcDirectionalLight(frag.normal,colorSample);

    // Sample the texture to obtain a color
    return (ambient_light + dir_light);
}