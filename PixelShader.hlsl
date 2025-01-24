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
    float3 normalizedNormal = normalize(input.normal);
    float3 lightDirection = normalize(lightPosition.xyz - input.worldPosition.xyz);
    float diffuseIntensity = max(dot(normalizedNormal, lightDirection), 0.0f);
    
    float3 reflection = reflect(-lightDirection, normalizedNormal);
    float3 vectorToCamera = normalize(cameraPosition - input.worldPosition);
    float specularIntensity = pow(max(dot(reflection, vectorToCamera), 0.0f), shininess);
    
    float4 ambientComponent = lightColor * ambientLightIntensity;
    float4 diffuseComponent = lightColor * diffuseIntensity;
    float4 specularComponent = lightColor * specularIntensity;

    return (ambientComponent + diffuseComponent + specularComponent) * shaderTexture.Sample(samplerState, input.uv);
}