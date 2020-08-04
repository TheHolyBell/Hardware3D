cbuffer CBuf : register(b0)
{
	matrix modelView;
	matrix modelViewProjection;
}

struct VSOut
{
	float3 PosW : POSITION;
	float3 Normal : NORMAL;
	float2 TexCoords : TEXCOORD;
	float4 PosH : SV_POSITION;
};

VSOut main(float3 pos : POSITION, float3 n : NORMAL, float2 texCoord : TEXCOORD)
{
	VSOut vso;

	vso.PosW = mul(float4(pos, 1.0f), modelView);
	vso.Normal = normalize(mul(n, (float3x3)modelView));
	vso.TexCoords = texCoord;
	vso.PosH = mul(float4(pos, 1.0f), modelViewProjection);

	return vso;
}