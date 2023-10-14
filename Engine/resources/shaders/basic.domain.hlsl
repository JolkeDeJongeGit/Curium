#define NUM_CONTROL_POINTS 4

struct Data
{
    float4x4 ViewProjection;
};
ConstantBuffer<Data> DataCB : register(b0);

struct PatchConstantData
{
    float edgeTessFactor[4] : SV_TessFactor;
    float insideTessFactor[2] : SV_InsideTessFactor;
};

struct HullToDomain
{
    float3 Position : POSITION;
    float4 Normal : NORMAL;
    float2 TextureCoord : TEXCOORD;
};

struct DomainToPixel
{
    float4 Position : SV_POSITION;
    float4 Normal : NORMAL;
    float2 TextureCoord : TEXCOORD;
};

[domain("quad")]
DomainToPixel main(PatchConstantData input, float2 domain : SV_DomainLocation, const OutputPatch<HullToDomain, NUM_CONTROL_POINTS> patch, uint patchID : SV_PrimitiveID)
{
    float u = domain.x;
    float v = domain.y;

    float2 uv0 = patch[0].TextureCoord;
    float2 uv1 = patch[1].TextureCoord;
    float2 uv2 = patch[2].TextureCoord;
    float2 uv3 = patch[3].TextureCoord;

    float2 leftUV = uv0 + v * (uv3 - uv0);
    float2 rightUV = uv1 + v * (uv2 - uv1);
    float2 texCoord = leftUV + u * (rightUV - leftUV);

    float3 pos0 = patch[0].Position;
    float3 pos1 = patch[1].Position;
    float3 pos2 = patch[2].Position;
    float3 pos3 = patch[3].Position;

    float3 leftPos = pos0 + v * (pos3 - pos0);
    float3 rightPos = pos1 + v * (pos2 - pos1);
    float4 pos = float4(leftPos + u * (rightPos - leftPos), 1);
    
    float4 nor0 = patch[0].Normal;
    float4 nor1 = patch[1].Normal;
    float4 nor2 = patch[2].Normal;
    float4 nor3 = patch[3].Normal;

    float4 leftNor = nor0 + v * (nor3 - nor0);
    float4 rightNor = nor1 + v * (nor2 - nor1);
    float4 nor =leftNor + u * (rightNor - leftNor);
    
    DomainToPixel output;
    output.Position = pos;
    output.Position = mul(DataCB.ViewProjection, output.Position);
    output.Normal = nor;
    output.TextureCoord = texCoord;

    return output;
}