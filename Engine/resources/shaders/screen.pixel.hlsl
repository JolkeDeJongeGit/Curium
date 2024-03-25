struct PixelIn
{
    float2 TexCoord : TexCoord;
};

Texture2D screenTexture : register(t0);
SamplerState LinearSampler : register(s0);

float3 RaySphere(float3 centerWorld, float atmosphereRadius, float3 scatterPoint, float3 lightDir)
{
    return float3(1.f, 1.f, 1.f);

}

float CalculateLight(float3 rayOrigin, float3 rayDir, float rayLength)
{
    float3 inScatterPoint = rayOrigin;
    int scatteringPoints = 10;
    
    float stepSize = rayLength / (scatteringPoints - 1);
    float inScatteredLight = 0;
    
    for (int i = 0; i < scatteringPoints; i++)
    {
        //float sunRayLength = RaySphere().y;
        //float sunRayOpticleDepth = OpticalDepth(inScatterPoint, dirToSun, sunRayLength);
        //float transmittance = exp(-sunRayOpticleDepth);
        inScatterPoint += rayDir * stepSize;

    }
    return 1.f;
}

float4 main(PixelIn pixelIn) : SV_TARGET
{
    float4 screen = screenTexture.Sample(LinearSampler, pixelIn.TexCoord);
    
    if (screen.a < 0.1)
    {
        discard;
    }
    
    return 1.f - float4(screen.rgb, 1.0f);
}