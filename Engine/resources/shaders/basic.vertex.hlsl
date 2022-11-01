struct ModelViewProjection
{
    matrix MVP;
    matrix Model;
};
ConstantBuffer<ModelViewProjection> ModelViewProjectionCB : register(b0);

struct VertexInput
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
    float2 TextureCoord : TEXCOORD;
};

struct PixelInput
{
    float4 Position : SV_Position;
    float3 Normal : NORMAL;
    float2 TextureCoord : TEXCOORD;
    float3 FragPos : COLOR0;
};

PixelInput main(VertexInput IN)
{
    PixelInput OUT;

    OUT.Position = mul(ModelViewProjectionCB.MVP, float4(IN.Position, 1));
    OUT.TextureCoord = IN.TextureCoord;
    OUT.Normal = mul(ModelViewProjectionCB.Model, float4(IN.Normal, 0.0f));
    
    float3 frag = mul(ModelViewProjectionCB.Model, float4(IN.Position, 0.0f));
    OUT.FragPos = frag;
    
    return OUT;
}