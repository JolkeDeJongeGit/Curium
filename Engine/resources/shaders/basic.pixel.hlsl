struct VertexShaderInput
{
    float4 PositionVS : POSITION;
    float3 NormalVS : NORMAL;
    float2 TexCoord   : TEXCOORD;
    float4 Position   : SV_Position;
};

SamplerState LinearSampler : register(s0);

float4 main(VertexShaderInput IN) : SV_Target
{
    return float4(0, 0, 0, 1);
}