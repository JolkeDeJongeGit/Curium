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

float3 LinearToSRGB(float3 x)
{
    // This is exactly the sRGB curve
    //return x < 0.0031308 ? 12.92 * x : 1.055 * pow(abs(x), 1.0 / 2.4) - 0.055;

    // This is cheaper but nearly equivalent
    return x < 0.0031308 ? 12.92 * x : 1.13005 * sqrt(abs(x - 0.00228)) - 0.13448 * x + 0.005719;
}

Texture2D Color : register(t0);
SamplerState LinearSampler : register(s0);

PixelOutput main(PixelInput Input)
{
    PixelOutput Output;

    float4 albedo = pow(Color.Sample(LinearSampler, Input.TextureCoord * 10000.f), 2.2f);
    Output.Color = albedo;
    //Output.Color = float4(255, 165, 0, 1);
    
    return Output;
}