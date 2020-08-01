cbuffer LightCBuf : register(b0)
{
	float3 lightPos;
	float3 ambient;
	float3 diffuseColor;
	float diffuseIntensity;
	float attConst;
	float attLin;
	float attQuad;
};

cbuffer ObjectCBuf : register(b1)
{
	float specularIntensity;
	float specularPower;
	bool normalMapEnabled;
	float padding[1];
};

cbuffer TransformCBuf : register(b2)
{
	matrix modelView;
	matrix modelViewProj;
};

Texture2D g_DiffuseMap : register(t0);
Texture2D g_NormalMap : register(t2);

SamplerState g_SamplerState : register(s0);


float4 main(float3 viewPos : Position, float3 n : Normal, float2 tc : Texcoord) : SV_Target
{
	// sample normal from map if normal mapping enabled
	if (normalMapEnabled)
	{
		// unpack normal data
		const float3 normalSample = g_NormalMap.Sample(g_SamplerState, tc).xyz;
		n.x = normalSample.x * 2.0f - 1.0f;
		n.y = -normalSample.y * 2.0f + 1.0f;
		n.z = -normalSample.z * 2.0f + 1.0f;
		n = mul(n, (float3x3) modelView);

		n = normalize(n);
	}
	// fragment to light vector data
	const float3 vToL = lightPos - viewPos;
	const float distToL = length(vToL);
	const float3 dirToL = vToL / distToL;
	// attenuation
	const float att = 1.0f / (attConst + attLin * distToL + attQuad * (distToL * distToL));
	// diffuse intensity
	const float3 diffuse = diffuseColor * diffuseIntensity * att * max(0.0f, dot(dirToL, n));
	// reflected light vector
	const float3 w = n * dot(vToL, n);
	const float3 r = w * 2.0f - vToL;
	// calculate specular intensity based on angle between viewing vector and reflection vector, narrow with power function
	const float3 specular = att * (diffuseColor * diffuseIntensity) * specularIntensity * pow(max(0.0f, dot(normalize(-r), normalize(viewPos))), specularPower);
	// final color
	return float4(saturate((diffuse + ambient) * g_DiffuseMap.Sample(g_SamplerState, tc).rgb + specular), 1.0f);
}