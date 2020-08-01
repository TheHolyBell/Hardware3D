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
    bool normalMapEnabled;
    float padding[3];
};

Texture2D g_DiffuseTexture : register(t0);
Texture2D g_SpecularMap : register(t1);
Texture2D g_NormalMap : register(t2);

SamplerState g_SamplerState : register(s0);


float4 main(float3 viewPos : Position, float3 n : Normal, float3 tan : Tangent, float3 bitan : Bitangent, float2 tc : Texcoord) : SV_Target
{
    // sample normal from map if normal mapping enabled
    if (normalMapEnabled)
    {
        // build the tranform (rotation) into tangent space
        const float3x3 tanToView = float3x3(
            normalize(tan),
            normalize(bitan),
            normalize(n)
        );
        // unpack normal data
        const float3 normalSample = g_NormalMap.Sample(g_SamplerState, tc).xyz;
        n = normalSample * 2.0f - 1.0f;
        // bring normal from tanspace into view space
        n = mul(n, tanToView);

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
    const float4 specularSample = g_SpecularMap.Sample(g_SamplerState, tc);
    const float3 specularReflectionColor = specularSample.rgb;
    const float specularPower = pow(2.0f, specularSample.a * 13.0f);
    const float3 specular = att * (diffuseColor * diffuseIntensity) * pow(max(0.0f, dot(normalize(-r), normalize(viewPos))), specularPower);
    // final color
    return float4(saturate((diffuse + ambient) * g_DiffuseTexture.Sample(g_SamplerState, tc).rgb + specular * specularReflectionColor), 1.0f);
}