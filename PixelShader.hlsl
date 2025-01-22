Texture2D shaderTexture : register(t0);
SamplerState samplerState : register(s0);

cbuffer ConstBuffer : register(b0)
{
    float4 lightPosition;
    float4 lightColor;
    float4 cameraPosition;
    float ambientLightIntensity;
    float shininess;
}

struct PixelShaderInput
{
    float4 position : SV_POSITION;
    float4 worldPosition : WORLD_POSITION;
    float3 normal : NORMAL;
    float2 uv : UV;
};

float4 main(PixelShaderInput input) : SV_TARGET
{
    
    
    float3 lightDirection = normalize(lightPosition.xyz - input.worldPosition.xyz);
    float3 normal = normalize(input.normal);
    float diffuseIntensity = max(dot(normal, lightDirection), 0.0);
    
    float4 ambientComponent = lightColor * ambientLightIntensity;
    float4 diffuseComponent = lightColor * diffuseIntensity;

    return (diffuseComponent) * shaderTexture.Sample(samplerState, input.uv);
}