Texture2D shaderTexture : register(t0);
SamplerState samplerState : register(s0);

struct PixelShaderInput
{
    float4 position : SV_POSITION;
    float4 worldPosition : WORLD_POSITION;
    float3 normal : NORMAL;
    float2 uv : UV;
};

float4 main(PixelShaderInput input) : SV_TARGET
{
    return shaderTexture.Sample(samplerState, input.uv);
}