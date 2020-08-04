#include "ShaderOps.hlsli"
#include "LightVectorData.hlsli"
#include "PointLight.hlsli"

cbuffer ObjectCBuf
{
    float specularPowerConst;
    bool hasGloss;
    float specularMapWeight;
};

Texture2D g_DiffuseMap : register(t0);
Texture2D g_SpecularMap : register(t1);

SamplerState g_SamplerState : register(s0);


float4 main(float3 viewFragPos : Position, float3 viewNormal : Normal, float2 tc : Texcoord) : SV_Target
{
    // normalize the mesh normal
    viewNormal = normalize(viewNormal);
    // fragment to light vector data
    const LightVectorData lv = CalculateLightVectorData(viewLightPos, viewFragPos);
    // specular parameters
    float specularPower = specularPowerConst;
    const float4 specularSample = g_SpecularMap.Sample(g_SamplerState, tc);
    const float3 specularReflectionColor = specularSample.rgb * specularMapWeight;
    if (hasGloss)
    {
        specularPower = pow(2.0f, specularSample.a * 13.0f);
    }
    // attenuation
    const float att = Attenuate(attConst, attLin, attQuad, lv.distToL);
    // diffuse light
    const float3 diffuse = Diffuse(diffuseColor, diffuseIntensity, att, lv.dirToL, viewNormal);
    // specular reflected
    const float3 specularReflected = Speculate(
        specularReflectionColor, 1.0f, viewNormal,
        lv.vToL, viewFragPos, att, specularPower
    );
    // final color = attenuate diffuse & ambient by diffuse texture color and add specular reflected
    return float4(saturate((diffuse + ambient) * g_DiffuseMap.Sample(g_SamplerState, tc).rgb + specularReflected), 1.0f);
}