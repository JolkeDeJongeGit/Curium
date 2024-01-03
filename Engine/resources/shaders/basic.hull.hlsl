#define NUM_CONTROL_POINTS 4
#define MIN_DETAIL 4
#define MAX_DETAIL 128

struct PatchTesselationFactors
{
    int edge;
    int inside;
};
ConstantBuffer<PatchTesselationFactors> tessFactors : register(b0);

struct Data
{
    float4x4 ViewProjection;
    float4 Eye;
}; // 48 bytes

ConstantBuffer<Data> DataCB : register(b0, space1);

struct VertexToHull
{
    float3 Position : POSITION0;
    float4 Normal : NORMAL;
    float2 TextureCoord : TEXCOORD;
};

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

PatchConstantData calculatePatchConstants(InputPatch<VertexToHull, NUM_CONTROL_POINTS> ip,
	uint PatchID : SV_PrimitiveID)
{
    PatchConstantData output;

    output.edgeTessFactor[0] = 16;
    output.edgeTessFactor[1] = 16;
    output.edgeTessFactor[2] = 16;
    output.edgeTessFactor[3] = 16;
    output.insideTessFactor[0] = 16;
    output.insideTessFactor[1] = 16;

    return output;
}

[domain("quad")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(NUM_CONTROL_POINTS)]
[patchconstantfunc("calculatePatchConstants")]
HullToDomain main(InputPatch<VertexToHull, NUM_CONTROL_POINTS> input, uint i : SV_OutputControlPointID)
{
    HullToDomain output;
    output.Position = input[i].Position;
    output.Normal = input[i].Normal;
    output.TextureCoord = input[i].TextureCoord;

    return output;
}