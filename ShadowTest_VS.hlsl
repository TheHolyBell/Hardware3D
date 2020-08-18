#include "Transform.hlsli"
cbuffer ShadowTransform
{
	matrix shadowView;
};

struct VSOut
{
	float3 PosV : POSITION;
	float3 NormalV : NORMAL;
	float2 TexCoords : TEXCOORD;
	float4 ShadowCamPos : SHADOWPOSITION;
	float4 PosH : SV_Position;
};

VSOut main( float3 pos : POSITION, float3 n : NORMAL, float2 tc : TEXCOORD )
{
	VSOut vso;

	vso.PosV = mul(float4(pos, 1.0f), modelView).xyz;
	vso.NormalV = mul(n, (float3x3)modelView);
	vso.PosH = mul(float4(pos, 1.0f), modelViewProj);
	vso.TexCoords = tc;

	const float4 shadowCamera = mul(float4(pos, 1.0f), model);
	const float4 shadowHomo = mul(shadowCamera, shadowView);
	vso.ShadowCamPos = shadowHomo * float4(0.5f, -0.5f, 1.0f, 1.0f) + (float4(0.5f, 0.5f, 0.0f, 0.0f) * shadowHomo.w);
	return vso;
}