TextureCube g_CubeMap : register(t0);
SamplerState g_SamplerState : register(s0);

struct VSOut
{
    float3 posW : Position;
    float4 posH : SV_Position;
};

float4 main(VSOut pin) : SV_TARGET
{
    return g_CubeMap.Sample(g_SamplerState, pin.posW);
}