// Pixel shader must specify a return semantic
// The entry point must be main

float4 main() : SV_TARGET
{
    return float4(0.0, 1.0, 1.0, 0.0);
}