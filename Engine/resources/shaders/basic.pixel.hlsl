struct ExtraData
{
    float4 CameraPosition;
    float4 LightDirection;
    float4 LightColor;
};
ConstantBuffer<ExtraData> Data : register(b1);

struct PixelInput
{
    float4 Position : SV_Position;
    float4 Normal : NORMAL;
    float4 FragPosition : FRAGPOSITION;
    float2 TextureCoord : TEXCOORD;
};

struct PixelOutput
{
    float4 Color : SV_Target0;
};

Texture2D Diffuse : register(t0);
SamplerState LinearSampler : register(s0);

static float PI = 3.14159265;
static float Inverse_PI =  1.f / PI;
static float Inverse_PI2 = 1.f / (2 * PI);

PixelOutput main(PixelInput Input)
{
    PixelOutput Output;
    
    float3 albedo = float3(1,0,0);
    float3 normal = Input.Normal.xyz;
    float3 ambient = float3(0.0, 0.0, 0.0);
    float alpha = 1.0;
    
    float ambientOcclusion = 1.0;

    // Texture
    {
        albedo = Diffuse.Sample(LinearSampler, Input.TextureCoord).rgb;
        albedo = pow(abs(albedo), 2.2);
            
        alpha = Diffuse.Sample(LinearSampler, Input.TextureCoord).a;
            
        ambient = albedo * 0.5;
    }
    
    // Light
    float3 lightDir = Data.LightDirection;
    float3 lightColor = Data.LightColor;
   
    float diff = max(dot(Input.Normal.xyz, lightDir), 0.2);
    float3 diffuse = diff * lightColor;
    
    float g = 1.0 / 2.2;
    ambient = pow(abs(ambient * diffuse), float3(g, g, g));
    
    ambient = clamp(ambient, float3(0.0, 0.0, 0.0), float3(1.0, 1.0, 1.0));

    Output.Color = float4(ambient * ambientOcclusion, alpha);
    return Output;
}