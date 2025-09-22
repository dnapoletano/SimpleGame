#include <metal_stdlib>
using namespace metal;

struct VertexData {
    float4 position;
    float2 uv;                      // UV coordinates
};

struct VertexPayload {              //Mesh Vertex Type
    float4 position [[position]];   //Qualified attribute
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
    payload.uv = vert.uv;
    return payload;
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

    // Sample the texture to obtain a color
    const auto colorSample = colorTexture.sample(textureSampler, frag.uv);
    return colorSample;
}