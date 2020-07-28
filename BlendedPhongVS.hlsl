cbuffer CBuf
{
	matrix modelView;
	matrix modelViewProjection;
};

struct VSOut
{
	float3 PosW : POSITION;
	float3 Normal : NORMAL;
	float3 Color : COLOR;
	float4 PosH : SV_POSITION;
};

VSOut main( float3 pos : POSITION, float3 n : NORMAL, float3 color : COLOR )
{
	VSOut vso;
	vso.PosW = mul(float4(pos, 1.0f), modelView);
	vso.Normal = mul(n, (float3x3)modelView);
	vso.Color = color;
	vso.PosH = mul(float4(pos, 1.0f), modelViewProjection);
	return vso;
}