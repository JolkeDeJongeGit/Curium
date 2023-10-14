#define NUM_CONTROL_POINTS 4

struct PatchTesselationFactors
{
    int edge;
    int inside;
};
ConstantBuffer<PatchTesselationFactors> tessFactors : register(b0);

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

PatchConstantData calculatePatchConstants()
{
    PatchConstantData output;

    output.edgeTessFactor[0] = tessFactors.edge;
    output.edgeTessFactor[1] = tessFactors.edge;
    output.edgeTessFactor[2] = tessFactors.edge;
    output.edgeTessFactor[3] = tessFactors.edge;
    output.insideTessFactor[0] = tessFactors.inside;
    output.insideTessFactor[1] = tessFactors.inside;

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