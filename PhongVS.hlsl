cbuffer CBuf
{
	matrix modelView;
	matrix MVP;
};

struct VSOut
{
	float3 PosW : POSITION;
	float3 Normal : NORMAL;
	float4 PosH : SV_POSITION;
};

VSOut main( float3 pos : POSITION, float3 n : NORMAL )
{
	VSOut vso;
	vso.PosW = mul(float4(pos, 1.0f), modelView).xyz;
	vso.Normal = mul(n, (float3x3)modelView);
	vso.PosH = mul(float4(pos, 1.0f), MVP);

	return vso;
}