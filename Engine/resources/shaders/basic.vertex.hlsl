
struct VertexInput
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
    float2 TextureCoord : TEXCOORD;
};

struct VertexOutput
{
    float4 Position : POSITION;
    float4 Normal : NORMAL;
    float2 TextureCoord : TEXCOORD;
};

VertexOutput main( VertexInput inInput )
{
    VertexOutput output;
    output.Position = float4(inInput.Position, 1.0f);
    //output.Position = mul(mul(mul(DataCB.Projection, DataCB.View), DataCB.Model), float4(inInput.Position, 1));
    output.TextureCoord = inInput.TextureCoord;
    //output.Normal = mul(DataCB.Model, float4(inInput.Normal, 1.0f));
    output.Normal = float4(inInput.Normal, 0.0f);
    
    return output;
}