// Pixel shader must specify a return semantic
// The entry point must be main

Texture2D shaderTexture;
SamplerState samplerType;

struct PixelInputType
{
    float4 pos : SV_POSITION;
    float2 texCoords : TEXCOORD0;
};

float4 main(PixelInputType input) : SV_TARGET
{
    return shaderTexture.Sample(samplerType, input.texCoords);
    //return float4(0.0F, 0.0F, input.texCoords.y, 1.0F);
}