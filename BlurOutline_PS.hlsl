Texture2D g_Texture : register(t0);
SamplerState g_SamplerState : register(s0);

cbuffer Kernel : register(b0)
{
	uint nTaps;
	float coefficients[15];
}

cbuffer Control : register(b1)
{
	bool horizontal;
}


float4 main(float2 uv : TEXCOORD) : SV_TARGET
{
	uint width, height;
	g_Texture.GetDimensions(width, height);
	float dx, dy;
	if (horizontal)
	{
		dx = 1.0f / width;
		dy = 0.0f;
	}
	else
	{
		dx = 0.0f;
		dy = 1.0f / height;
	}
	const int r = nTaps / 2;

	float accAlpha = 0.0f;
	float3 maxColor = float3(0.0f, 0.0f, 0.0f);
	for (int i = -r; i <= r; ++i)
	{
		const float2 tc = uv + float2(dx * i, dy * i);
		const float4 s = g_Texture.Sample(g_SamplerState, tc).rgba;
		const float coef = coefficients[i + r];
		accAlpha += s.a * coef;
		maxColor = max(s.rgb, maxColor);
	}

	return float4(maxColor, accAlpha);
}