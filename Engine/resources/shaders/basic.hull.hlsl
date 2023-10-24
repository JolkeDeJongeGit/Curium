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
    //float4 frustum[6];
    //float dummy;
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

float CalcTessFactor(float3 p)
{
    float d = distance(p, DataCB.Eye.xyz);

    float s = saturate((d - 16.0f) / (256.0f - 16.0f));
    return pow(2, (lerp(6, 0, s)));
}

PatchConstantData calculatePatchConstants(InputPatch<VertexToHull, NUM_CONTROL_POINTS> ip,
	uint PatchID : SV_PrimitiveID)
{
    PatchConstantData output;
        
    //output.edgeTessFactor[0] = tessFactors.edge;
    //output.edgeTessFactor[1] = tessFactors.edge;
    //output.edgeTessFactor[2] = tessFactors.edge;
    //output.edgeTessFactor[3] = tessFactors.edge;
    //output.insideTessFactor[0] = tessFactors.inside;
    //output.insideTessFactor[1] = tessFactors.inside;
    
    float4 view0 = mul(DataCB.ViewProjection, float4(ip[0].Position, 1));
    float4 view1 = mul(DataCB.ViewProjection, float4(ip[1].Position, 1));
    float4 view2 = mul(DataCB.ViewProjection, float4(ip[2].Position, 1));
    float4 view3 = mul(DataCB.ViewProjection, float4(ip[3].Position, 1));
    
    float3 e0 = 0.5f * (view0 + view2);
    float3 e1 = 0.5f * (view0 + view1);
    float3 e2 = 0.5f * (view1 + view3);
    float3 e3 = 0.5f * (view2 + view3);
    float3 c = 0.25f * (view0 + view1 + view2 + view3);

    output.edgeTessFactor[0] = clamp_distance(e0);
    output.edgeTessFactor[1] = clamp_distance(e1);
    output.edgeTessFactor[2] = clamp_distance(e2);
    output.edgeTessFactor[3] = clamp_distance(e3);
    output.insideTessFactor[0] = clamp_distance(c);
    output.insideTessFactor[1] = output.insideTessFactor[0];

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