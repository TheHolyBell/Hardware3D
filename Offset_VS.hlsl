#include "Transform.hlsli"

cbuffer Offset
{
	float offset;
};

float4 main(float3 pos : Position, float3 n : NORMAL) : SV_POSITION
{
	return mul(float4(pos + n * offset,1.0f), modelViewProj);
}