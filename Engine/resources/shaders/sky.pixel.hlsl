struct PixelIN
{
    float2 TexCoord : TexCoord;
};

Texture2D Sky : register(t0);
SamplerState LinearSampler : register(s0);


float4 main(PixelIN pixelIn) : SV_TARGET
{
    float3 sky = Sky.Sample(LinearSampler, pixelIn.TexCoord).rgb;
    
    float g = 0.454545454545;
    float3 color = pow(abs(sky), float3(g, g, g));
    
    return float4(color, 1.0f);
}