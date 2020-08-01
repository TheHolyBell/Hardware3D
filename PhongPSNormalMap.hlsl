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

Texture2D g_DiffuseMap : register(t0);
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
        // unpack the normal from map into tangent space        
        const float3 normalSample = g_NormalMap.Sample(g_SamplerState, tc).xyz;
        n = normalSample * 2.0f - 1.0f;
        n.y = -n.y;
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
    const float3 r = reflect(-vToL, n);
    // calculate specular intensity based on angle between viewing vector and reflection vector, narrow with power function
    const float3 specular = att * (diffuseColor * diffuseIntensity) * specularIntensity * pow(max(0.0f, dot(normalize(-r), normalize(viewPos))), specularPower);
    // final color
    return float4(saturate((diffuse + ambient) * g_DiffuseMap.Sample(g_SamplerState, tc).rgb + specular), 1.0f);
}