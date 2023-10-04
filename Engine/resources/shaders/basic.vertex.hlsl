struct Data
{
    float4x4 View;
    float4x4 Projection;
    float4x4 Model;
};
ConstantBuffer<Data> DataCB : register(b0);

struct VertexInput
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
    float2 TextureCoord : TEXCOORD;
};

struct VertexOutput
{
    float4 Position : SV_Position;
    float4 Normal : NORMAL;
    float2 TextureCoord : TEXCOORD;
};

VertexOutput main( VertexInput inInput )
{
    VertexOutput output;

    const float4 worldPosition = mul(DataCB.Model, float4(inInput.Position, 1));
    output.Position = mul(mul(DataCB.Projection, DataCB.View), worldPosition);
    output.TextureCoord = inInput.TextureCoord;
    //output.Normal = mul(DataCB.Model, float4(inInput.Normal, 0.0f));
    output.Normal = float4(inInput.Normal, 0.0f);
    
    return output;
}