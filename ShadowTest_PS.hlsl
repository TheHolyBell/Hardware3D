#include "ShaderOps.hlsli"
#include "LightVectorData.hlsli"

#include "PointLight.hlsli"

cbuffer ObjectCBuf
{
    float3 specularColor;
    float specularWeight;
    float specularGloss;
};

Texture2D g_DiffuseMap : register(t0);
Texture2D g_ShadowMap : register(t3);

SamplerState g_SamplerState : register(s0);
SamplerState g_ShadowSampler : register(s1);


float4 main(float3 viewFragPos : Position, float3 viewNormal : Normal, float2 tc : Texcoord, float4 spos : ShadowPosition) : SV_Target
{
    float3 diffuse;
    float3 specular;

    // shadow map test
    spos.xyz = spos.xyz / spos.w;
    float bias = 0.005f;
    if (g_ShadowMap.Sample(g_ShadowSampler, spos.xy).r > spos.z - bias)
    {
        // renormalize interpolated normal
        viewNormal = normalize(viewNormal);
        // fragment to light vector data
        const LightVectorData lv = CalculateLightVectorData(viewLightPos, viewFragPos);
        // attenuation
        const float att = Attenuate(attConst, attLin, attQuad, lv.distToL);
        // diffuse
        diffuse = Diffuse(diffuseColor, diffuseIntensity, att, lv.dirToL, viewNormal);
        // specular
        specular = Speculate(diffuseColor * diffuseIntensity * specularColor, specularWeight, viewNormal, lv.vToL, viewFragPos, att, specularGloss);
    }
    else
    {
        diffuse = specular = float3(0.0f, 0.0f, 0.0f);
    }
    // final color
    return float4(saturate((diffuse + ambient) * g_DiffuseMap.Sample(g_SamplerState, tc).rgb + specular), 1.0f);
}