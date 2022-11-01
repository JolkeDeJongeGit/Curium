struct PixelInput
{
    float4 Position : SV_Position;
    float3 Normal : NORMAL;
    float2 TextureCoord : TEXCOORD;
    float3 FragPos : COLOR0;
};

SamplerState LinearSampler : register(s0);

float4 main(PixelInput IN) : SV_Target
{
    return float4(0, 0, 0, 1);
}