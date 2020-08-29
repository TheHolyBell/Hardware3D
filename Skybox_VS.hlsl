cbuffer TransformCBuf : register(b0)
{
	matrix viewProj;
};

struct VSOut
{
    float3 posW : Position;
    float4 posH : SV_Position;
};

VSOut main(float3 pos : Position)
{
    VSOut vso;
    vso.posW = pos;
    vso.posH = mul(float4(pos, 0.0f), viewProj);
    // make sure that the depth after w divide will be 1.0 (so that the z-buffering will work)
    vso.posH.z = vso.posH.w;
    return vso;
}