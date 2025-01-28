cbuffer ConstantBuffer : register(b0)
{
    float4x4 worldMatrix;
    float4x4 viewProjectionMatrix;
};

struct VertexShaderInput
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 uv : UV;
};

struct VertexShaderOutput
{
    float4 position : SV_POSITION;
    float4 worldPosition : WORLD_POSITION;
    float4 normal : NORMAL;
    float2 uv : UV;
};

VertexShaderOutput main(VertexShaderInput input)
{
    VertexShaderOutput output;
    output.worldPosition = mul(float4(input.position, 1.0f), worldMatrix);
    output.position = mul(output.worldPosition, viewProjectionMatrix);
    output.normal = normalize(float4(mul(float4(input.normal, 1.0f), worldMatrix).xyz, 0.0f));
    output.uv = input.uv;
    return output;
}