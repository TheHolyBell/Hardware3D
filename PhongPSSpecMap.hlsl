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

Texture2D g_DiffuseTexture : register(t0);
Texture2D g_SpecularTexture : register(t1);

SamplerState g_SamplerState : register(s0);

struct VSOut
{
    float3 PosW : POSITION;
    float3 Normal : NORMAL;
    float2 TexCoords : TEXCOORD;
    float4 PosH : SV_POSITION;
};

float4 main(VSOut pin) : SV_Target
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
    const float4 specularSample = g_SpecularTexture.Sample(g_SamplerState, tc);
    const float3 specularReflectionColor = specularSample.rgb;
    const float specularPower = pow(2.0f, specularSample.a * 13.0f);
    const float3 specular = att * (diffuseColor * diffuseIntensity) * pow(max(0.0f, dot(normalize(-r), normalize(worldPos))), specularPower);
    // final color
    return float4(saturate((diffuse + ambient) * g_DiffuseTexture.Sample(g_SamplerState, tc).rgb + specular * specularReflectionColor), 1.0f);
}