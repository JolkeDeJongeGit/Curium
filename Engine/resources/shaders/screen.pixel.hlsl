struct PixelIn
{
    float2 TexCoord : TexCoord;
};

struct Data
{
    float4 eye; // 4 * 4 = 16
    float4 dir; // 4 * 4 = 16
}; // 32 bytes
ConstantBuffer<Data> DataCB : register(b0);

Texture2D screenTexture : register(t0);
Texture2D depthTexture : register(t1);
SamplerState LinearSampler : register(s0);

float2 RaySphere(float3 centerWorld, float atmosphereRadius, float3 rayOrigin, float3 rayDir)
{
 
    float3 directionSphere = rayOrigin - centerWorld; // gives the distance from the sphere to the ray orgin
    float a = 1;
    float b = dot(directionSphere, rayDir); // The projection length of the ray vector 
    float c = directionSphere * directionSphere - atmosphereRadius * atmosphereRadius; // It gets the distance from the ray to the shell.
    float discriminant = b * b - a * c; // to calculate the discriminant b*b - 4ac
    if (discriminant > 0.f) // when the discriminant is bigger than 0 then we know it hits the sphere
    {
        float sqrtfd = sqrt(discriminant);
        float inverseA = 1.f / a;

        float near = max(0, (-b - sqrtfd) * inverseA); //We will calculate the t value using the quadratic formula
        float far = (-b + sqrtfd) * inverseA; //We will calculate the t value using the quadratic formula
        
        if (far >= 0)
        {
            return float2(near, far - near);
        }
    }


    return float2(3.402823466e+38F, 0);
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
    float4 depth = depthTexture.Sample(LinearSampler, pixelIn.TexCoord);
    
    float3 sphereOrigin = float3(0.f,0.f,0.f);

    float2 hitData = RaySphere(sphereOrigin, 10.f, DataCB.eye.xyz, DataCB.dir.xyz);
    
    //return float4(0.f, 0.f, 0.f, 0.f);
    //return hitData.x / (hitData.y * 2);
    return float4(1.f - screen.x, 1.f - screen.y, 1.f - screen.z, 1);
    //return float4(1.f - depth.x, 1.f - depth.x, 1.f - depth.x, 1);
}