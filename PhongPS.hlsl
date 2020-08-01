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
	float3 worldPos = pin.PosW;
	float3 n = normalize(pin.Normal);
	float2 tc = pin.TexCoords;

	// fragment to light vector data
	const float3 vToL = lightPos - worldPos;
	const float distToL = length(vToL);
	const float3 dirToL = vToL / distToL;
	// attenuation
	const float att = 1.0f / (attConst + attLin * distToL + attQuad * (distToL * distToL));
	// diffuse intensity
	const float3 diffuse = diffuseColor * diffuseIntensity * att * max(0.0f, dot(dirToL, n));
	// reflected light vector
	const float3 r = reflect(-vToL, n);
	// calculate specular intensity based on angle between viewing vector and reflection vector, narrow with power function
	const float3 specular = att * (diffuseColor * diffuseIntensity) * specularIntensity * pow(max(0.0f, dot(normalize(-r), normalize(worldPos))), specularPower);
	// final color
	return float4(saturate((diffuse + ambient) * g_Texture.Sample(g_SamplerState, tc).rgb + specular), 1.0f);
}