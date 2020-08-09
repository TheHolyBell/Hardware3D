Texture2D g_Texture : register(t0);
SamplerState g_SamplerState : register(s0);

static const int r = 3;
static const float divisor = (2 * r + 1) * (2 * r + 1);

float4 main(float2 uv : Texcoord) : SV_Target
{
    //return float4(1.0f, 1.0f, 1.0f, 1.0f) - g_Texture.Sample(g_SamplerState, uv);
    uint width, height;
    g_Texture.GetDimensions(width, height);
    const float dx = 1.0f / width;
    const float dy = 1.0f / height;
    float4 acc = float4(0.0f, 0.0f, 0.0f, 0.0f);
    for (int y = -r; y <= r; y++)
    {
        for (int x = -r; x <= r; x++)
        {
            const float2 tc = uv + float2(dx * x, dy * y);
            acc += g_Texture.Sample(g_SamplerState, tc).rgba;
        }
    }
    return acc / divisor;
}