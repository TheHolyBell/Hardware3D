cbuffer CBuf : register(b0)
{
	matrix modelView;
	matrix MVP;
}

struct VSOut
{
	float3 PosV : POSITION;
	float3 Normal : NORMAL;
	float3 Tangent : TANGENT;
	float3 Bitangent : BITANGENT;
	float2 TexCoords : TEXCOORD;
	float4 PosH : SV_POSITION;
};

VSOut main(float3 pos : POSITION, float3 n : NORMAL, float3 tan : TANGENT, float3 bitan : BITANGENT, float2 tc : TEXCOORD)
{
	VSOut vso;
	vso.PosV = mul(float4(pos, 1.0f), modelView).xyz;
	vso.Normal = mul(n, (float3x3)modelView);
	vso.Tangent = mul(tan, (float3x3)modelView);
	vso.Bitangent = mul(bitan, (float3x3)modelView);
	vso.PosH = mul(float4(pos, 1.0f), MVP);
	vso.TexCoords = tc;

	return vso;
}