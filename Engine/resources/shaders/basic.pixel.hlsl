struct PixelInput
{
    float4 Position : SV_Position;
    float4 Normal : NORMAL;
    float2 TextureCoord : TEXCOORD;
};

struct PixelOutput
{
    float4 Color : SV_Target0;
};

Texture2D Color : register(t0);
SamplerState LinearSampler : register(s0);

PixelOutput main(PixelInput Input)
{
    PixelOutput Output;
    float4 albedo = pow(Color.Sample(LinearSampler, Input.TextureCoord), 2.2f);
    Output.Color = albedo;

    return Output;
}