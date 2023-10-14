struct PixelInput
{
    float4 Position : SV_Position;
    float4 Normal : NORMAL;
    float2 TextureCoord : TEXCOORD;
    //float4 Normal : NORMAL;
    //float2 TextureCoord : TEXCOORD;
};

struct PixelOutput
{
    float4 Color : SV_Target0;
};

Texture2D Color : register(t0);
SamplerState LinearSampler : register(s0);

PixelOutput main(PixelInput Input)
{
    PixelOutput Output;
    //float4 albedo = pow(Color.Sample(LinearSampler, Input.TextureCoord), 2.2f);
    //if (Input.Normal[0] == 0 && Input.Normal[1] == 0 && Input.Normal[2] == 0)
    //{
        
    //    Output.Color = float4(1,1, 1, 1);
    //}
    //else
    //{
    //    = Input.Normal;
        
    //}

    Output.Color = float4(Input.TextureCoord + 1.0, 0.000, 1.0) / 2.0;
    
    return Output;
}