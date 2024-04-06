#define NUM_CONTROL_POINTS 4

struct TransformData
{
    matrix ViewProjection;
    matrix Model;
};
ConstantBuffer<TransformData> Transform : register(b0, space1);

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
    float4 FragPosition : FRAGPOSITION;
    float2 TextureCoord : TEXCOORD;
};

Texture2D<float4> heightmap : register(t1);
SamplerState LinearSampler : register(s0);

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
    //output.Normal = nor;
    
    float4 normal = nor;
    

    float displacement = heightmap.SampleLevel(LinearSampler, texCoord, 0.0f).r;
    float4 normaldisplacement = normal * displacement;
// Displace the vertex along the normal direction
    float3 worldPos = pos.xyz + normaldisplacement.xyz * 1500.f;
    //float3 worldPos = pos.xyz ; // Displace the vertex along the y-axis

    output.Position = mul(mul(Transform.ViewProjection, Transform.Model), float4(worldPos, 1.f)) ;
    output.Normal =  normaldisplacement;
    output.FragPosition = mul(Transform.Model, float4(worldPos, 1.f));
    output.TextureCoord = texCoord;

    return output;
}