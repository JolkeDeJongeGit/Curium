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
    float4 lightDir = float4(0, 0.5f, 1.f, 1.f);
    float4 lightColor = float4(1, 1, 1, 1);
    
    float ambientStrength = 0.01f;
    float4 ambient = ambientStrength * lightColor;
    
    
    float diff = max(dot(Input.Normal, lightDir), 0.0);
    float4 diffuse = diff * lightColor;
    
    
    //float4 albedo = Color.Sample(LinearSampler, Input.TextureCoord * 0.1);
    float4 albedo = float4(0.5450, 0.2705, 0.075, 1 );
    Output.Color = (ambient + diffuse) * albedo;
    return Output;
}