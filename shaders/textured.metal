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

struct AmbientLight {
   float strength;
   float4 colour;
};

struct DirectionalLight {
   float strength;
   float4 colour;
   float3 direction;
};

struct PointLight {
   float strength;
   float4 colour;
   float3 position;
};

struct VertexPayload {              //Mesh Vertex Type
    float4 position [[position]];   //Qualified attribute
    float3 normal;
    float3 tangent;
    float3 bitangent;
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
    payload.uv = vert.uv;
    return payload;
}


float4 calcAmbientLight(constant AmbientLight& al, float4 albedo) {
    return float4(al.strength * al.colour.rgb * albedo.rgb,1.0f);
}

float4 calcDirectionalLight(constant DirectionalLight& dl, float3 normal, float4 albedo) {
    const auto norm = simd::normalize(-dl.direction);
    const auto diff = max(dot(normal,norm),0.0);
    const auto dir_colour = float4((diff * dl.colour).xyz,1.0);
    return float4(dl.strength * dir_colour.rgb * albedo.rgb,1.0f);
}

float4 calcPointLight(constant PointLight& pl,
                      constant float3& cameraPosition,
                      float3 fragPosition,
                      float3 normal,
                      float4 albedo,
                      float4 specular){

    const auto direction = normalize(pl.position - fragPosition);
    const auto diffuse = max(dot(normal,direction),0.0f);
    const auto distance = length(pl.position - fragPosition);
    const auto reflect_direction = reflect(-direction, normal);
    const auto viewDir = normalize(cameraPosition - fragPosition);
    const auto spec = pow(max(dot(viewDir, reflect_direction), 0.0),2);
    const auto specular_light = (specular.r * spec) * albedo * 50.0f;
    return float4(pl.strength * albedo.rgb * ((diffuse + specular_light.rgb) / distance / distance) * pl.colour.rgb, 1.0);
}


float3 fresnelSchlick(float cosTheta, float3 F0)
{
    // Schlick's approximation of Fresnel
    return F0 + (1.0f - F0) * pow(1.0f - cosTheta, 5.0f);
}

float distributionGGX(float3 N, float3 H, float roughness)
{
    float a      = roughness * roughness;
    float a2     = a * a;
    float NdotH  = max(dot(N, H), 0.0f);
    float NdotH2 = NdotH * NdotH;

    float denom = (NdotH2 * (a2 - 1.0f) + 1.0f);
    return a2 / (M_PI * denom * denom);
}

float geometrySmith(float3 N, float3 V, float3 L, float roughness)
{
    float r = (roughness + 1.0f);
    float k = (r * r) / 8.0f;   // from UE4

    float NdotV = max(dot(N, V), 0.0f);
    float NdotL = max(dot(N, L), 0.0f);

    float ggxV = NdotV / (NdotV * (1.0f - k) + k);
    float ggxL = NdotL / (NdotL * (1.0f - k) + k);

    return ggxV * ggxL;
}

float3 calcSpecularGGX(float3 lightPosition,
                       float3 cameraPosition,
                       float3 fragPosition,
                       float3 fragNormal,
                       float3 lightColor, float3 albedo,
                       float roughness, float metalness)
{
    float3 N = normalize(fragNormal);
    float3 V = normalize(cameraPosition - fragPosition);
    float3 L = normalize(lightPosition - fragPosition);

    float3 H = normalize(V + L);   // half vector

    // Fresnel term: base reflectivity
    float3 F0 = mix(float3(0.04), albedo, metalness);
    float3 F  = fresnelSchlick(max(dot(H, V), 0.0), F0);

    float D = distributionGGX(N, H, roughness);
    float G = geometrySmith(N, V, L, roughness);

    float NdotL = max(dot(N, L), 0.0);
    float NdotV = max(dot(N, V), 0.0);

    float3 numerator   = D * F * G;
    float denominator  = 4.0 * NdotV * NdotL + 0.001;
    float3 specular    = numerator / denominator;

    // energy conservation: diffuse gets reduced by Fresnel
    float3 kS = F;
    float3 kD = (1.0 - kS) * (1.0 - metalness);

    float3 diffuse = kD * albedo / M_PI;

    float attenuation = 1.0f / (dot(lightPosition - fragPosition,lightPosition - fragPosition)) ;

    return (diffuse + specular) * lightColor * NdotL * attenuation;
}

/*
    The vertex qualifier registers this function in the vertex stage of the Metal API.

    See page 104 of the metal spec,
    table 5.5: Attributes for fragment function input arguments,
    for more info.
*/
fragment float4 fragmentMain(VertexPayload frag [[stage_in]], texture2d_array<float> colorTexture [[texture(0)]],
                             texturecube<float> skyBox [[texture(1)]],
                            constant AmbientLight& al           [[buffer(2)]],
                            constant DirectionalLight& dl       [[buffer(3)]],
                            constant PointLight& pl             [[buffer(4)]],
                            constant float3& cameraPosition     [[buffer(5)]])
{
    const auto TBN = float3x3(frag.tangent, frag.bitangent, frag.normal);
    constexpr sampler textureSampler (mag_filter::linear,min_filter::linear);

    const auto colorSample      = colorTexture.sample(textureSampler, frag.uv, 0);
    const auto specularSample   = colorTexture.sample(textureSampler, frag.uv, 1);
    const auto roughnessSample   = colorTexture.sample(textureSampler, frag.uv, 2);

    const auto ambient_light    = calcAmbientLight(al, colorSample);

    const auto normalMap        = normalize(
        colorTexture.sample(textureSampler, frag.uv, 3).rgb * 2.0f - 1.0f
    );

    const auto normal           = normalize(TBN * normalMap);
    const auto dir_light        = calcDirectionalLight(dl, normal, colorSample);

    const auto point_light = calcSpecularGGX(pl.position, cameraPosition, frag.wPosition.xyz, normal,
                                             pl.colour.rgb * pl.strength, // light color/intensity
                                             colorSample.rgb, // albedo from texture
                                             roughnessSample.r, specularSample.r);

    constexpr sampler skyBoxSampler(address::clamp_to_edge, filter::linear);
    const auto Ivec = normalize(pl.position - cameraPosition);
    const auto Rvec = refract(Ivec, normalize(normal),1.0f/1.52f);
    const auto skyBoxLighting =  skyBox.sample(skyBoxSampler,Rvec);
    // Sample the texture to obtain a color
    const auto hdrColor = float4((ambient_light.rgb + dir_light.rgb + point_light + skyBoxLighting.rgb),1.0f);
    // constexpr auto gamma = 2.2f;
    // constexpr auto exposure = 0.3f;
    // auto mappedColor = float3(1.0f) - exp(-hdrColor.rgb * exposure);
    // mappedColor = pow(mappedColor, float3(1.0f / gamma));
    // return float4(mappedColor,1.0f);
    return hdrColor;
}
