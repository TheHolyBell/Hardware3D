struct VSOut
{
	float2 uv : TEXCOORD;
	float4 PosH : SV_POSITION;
};

VSOut main(float2 pos : POSITION)
{
	VSOut vso;
	vso.PosH = float4(pos, 0.0f, 1.0f);
	vso.uv = float2((pos.x + 1) / 2.0f, -(pos.y - 1) / 2.0f);
	return vso;
}