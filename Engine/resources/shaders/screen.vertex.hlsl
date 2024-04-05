struct VertexPosColor
{
    float3 Position : POSITION;
    float2 TexCoord : TEXCOORD;
};

struct VertexOutput
{
    float2 TexCoord : TexCoord;
    float4 Position : SV_POSITION;
};

VertexOutput main(VertexPosColor vertextIn)
{
    VertexOutput vertexOut;
    vertexOut.Position = float4(vertextIn.Position, 1.0);
    vertexOut.TexCoord = vertextIn.TexCoord;
    
    return vertexOut;
}