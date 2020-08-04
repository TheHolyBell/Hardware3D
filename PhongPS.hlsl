#include "ShaderOps.hlsli"
#include "LightVectorData.hlsli"
#include "PointLight.hlsli"

cbuffer ObjectCBuf
{
	float specularIntensity;
	float specularPower;
	float padding[2];
};

struct VSOut
{
	float3 PosW : POSITION;
	float3 Normal : NORMAL;
	float2 TexCoords : TEXCOORD;
	float4 PosH : SV_POSITION;
};

Texture2D g_Texture : register(t0);
SamplerState g_SamplerState : register(s0);

float4 main(VSOut pin) : SV_TARGET
{
	float3 viewPos = pin.PosW;
	float3 n = normalize(pin.Normal);
	float2 tc = pin.TexCoords;

	const LightVectorData lv = CalculateLightVectorData(viewLightPos, viewPos);

	float att = Attenuate(attConst, attLin, attQuad, lv.distToL);

	const float3 diffuse = Diffuse(diffuseColor, diffuseIntensity, att, lv.dirToL, n);

	const float3 specular = Speculate(diffuseColor, diffuseIntensity, n, lv.vToL, viewPos, att, specularPower);
	
	return float4(saturate((diffuse + ambient) * g_Texture.Sample(g_SamplerState, tc).rgb + specular), 1.0f);
}