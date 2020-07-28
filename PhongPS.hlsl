cbuffer LightCBuf
{
	float3 lightPos;
	float3 ambient;
	float3 diffuseColor;
	float diffuseIntensity;
	float attConst;
	float attLin;
	float attQuad;
};

cbuffer ObjectCBuf
{
	float3 materialColor;
	float specularIntensity;
	float specularPower;
};

struct VSOut
{
	float3 PosW : POSITION;
	float3 Normal : NORMAL;
	float4 PosH : SV_POSITION;
};

float4 main(VSOut pin) : SV_TARGET
{
	// Fragment to light vector data
	float3 vToL = lightPos - pin.PosW;
	float distToL = length(vToL);
	float3 dirToL = vToL / distToL;

	// Diffuse attenuation
	float att = 1.0f / (attConst + distToL * attLin + attQuad * pow(distToL, 2));
	// Diffuse intensity
	float3 diffuse = diffuseColor * diffuseIntensity * att * max(0.0f, dot(dirToL, pin.Normal));

	// Reflected light vector
	float3 r = reflect(-vToL, pin.Normal);

	// Calculate specular intensity on angle between viewing vector and reflection vector, narrow with power function
	const float3 specular = att * (diffuseColor * diffuseIntensity) * specularIntensity * pow(max(0.0f, dot(normalize(-r), normalize(pin.PosW))), specularPower);
	return float4(saturate(diffuse + ambient + specular) * materialColor, 1.0f);
}