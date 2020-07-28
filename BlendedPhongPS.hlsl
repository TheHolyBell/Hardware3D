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
	float3 Color : COLOR;
	float4 PosH : SV_POSITION;
};

float4 main(VSOut pin) : SV_TARGET
{
	// fragment to light vector data
	const float3 vToL = lightPos - pin.PosW;
	const float distToL = length(vToL);
	const float3 dirToL = vToL / distToL;
	// attenuation
	const float att = 1.0f / (attConst + attLin * distToL + attQuad * (distToL * distToL));
	// diffuse intensity
	const float3 diffuse = diffuseColor * diffuseIntensity * att * max(0.0f,dot(dirToL,pin.Normal));
	// reflected light vector
	const float3 r = reflect(-vToL, pin.Normal);
	// calculate specular intensity based on angle between viewing vector and reflection vector, narrow with power function
	const float3 specular = att * (diffuseColor * diffuseIntensity) * specularIntensity * pow(max(0.0f,dot(normalize(-r),normalize(pin.PosW))),specularPower);
	// final color
	return float4(saturate((diffuse + ambient + specular) * pin.Color), 1.0f);
}