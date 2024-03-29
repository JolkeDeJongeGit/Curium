#define FLT_MAX 3.402823466e+38
#define NUM_SCATTERING_POINTS 10
#define NUM_DENSITY_POINTS 10
#define PI 3.141592

struct PixelIn
{
    float2 TexCoord : TexCoord;
};

struct CameraData
{
    float4x4 viewMatrix;
    float4x4 inverseProjectionMatrix;
    float3 position;
    float _near;
    float _far;
}; 
ConstantBuffer<CameraData> Camera : register(b0);

struct Planet
{
    float3 position;
    float planetRadius;
    float atmosphereRadius;
    float atmosphereFalloff;
};
ConstantBuffer<Planet> World : register(b1);

struct Light
{
    float3 position;
    float3 direction;
    float colorHex;
    float intensity;
};
ConstantBuffer<Light> Sun : register(b2);

Texture2D screenTexture : register(t0);
Texture2D depthTexture : register(t1);
SamplerState LinearSampler : register(s0);


float2 RaySphere(float3 centerWorld, float atmosphereRadius, float3 rayOrigin, float3 rayDir)
{
    float3 directionSphere = rayOrigin - centerWorld; // Distance from the sphere center to the ray origin
    float a = dot(rayDir, rayDir); // The length of the ray direction squared (should be 1 if normalized)
    float b = dot(directionSphere, rayDir); // The projection length of the ray vector
    float c = dot(directionSphere, directionSphere) - atmosphereRadius * atmosphereRadius; // Distance from the ray to the sphere center squared minus the sphere radius squared
    float discriminant = b * b - a * c; // Calculate the discriminant b*b - a*c

    if (discriminant > 0.0f) // If the discriminant is positive, the ray intersects the sphere
    {
        float sqrtDiscriminant = sqrt(discriminant);
        float inverseA = 1.0f / a;

        float near = max(0.0f, (-b - sqrtDiscriminant) * inverseA); // Calculate the first intersection point
        float far = (-b + sqrtDiscriminant) * inverseA; // Calculate the second intersection point

        return float2(near, far - near); // Return the intersection points' distances from the ray origin and the length of the intersection segment
    }

    return float2(FLT_MAX, 0); // No intersection, return maximum float value
}

float3 ScreenToWorld(float3 pos)
{
    float4 posP = float4(pos.x * 2.0 - 1.0, (1.0f - pos.y) * 2.0f - 1.0f, pos.z * 2.0 - 1.0, 1.0); // Don't forget to invert Y.
    float4 posVS = mul(Camera.inverseProjectionMatrix, posP); // Clip to view space
    posVS /= posVS.w; // Perspective divide
    float4 posWS = mul(Camera.viewMatrix, posVS); // View to world space
    return posWS.xyz;
}

float LinearEyeDepth(float depthNonLinear)
{
    return (2.0 * Camera._near) / (Camera._far + Camera._near - depthNonLinear * (Camera._far - Camera._near));
}

float DensityAtPoint(float3 densitySamplePoint)
{
    float heightAboveSurface = length(densitySamplePoint - World.position.xyz) - World.planetRadius;
    float height01 = heightAboveSurface / (World.atmosphereRadius - World.planetRadius);
    float localDensity = exp(-height01 * World.atmosphereFalloff) * (1 - height01);
    return localDensity;
}

float OpticalDepth(float3 rayOrigin, float3 rayDir, float rayLength)
{
    float3 densitySamplePoint = rayOrigin;
    float stepSize = rayLength / (NUM_DENSITY_POINTS - 1);
    float opticalDepth = 0; 
    
    for (int i = 0; i < NUM_DENSITY_POINTS; i++)
    {
        float localDensity = DensityAtPoint(densitySamplePoint);
        opticalDepth += localDensity * stepSize;
        densitySamplePoint += rayDir * stepSize;
    }
    
    return opticalDepth;
}

float CalculateLight(float3 rayOrigin, float3 rayDir, float rayLength)
{
    float3 inScatteredPoint = rayOrigin;
    float stepSize = rayLength / (NUM_SCATTERING_POINTS - 1);
    float inScatteredLight = 0;
    
    for (int i = 0; i < NUM_SCATTERING_POINTS; i++)
    {
        float3 dirToSun = normalize(Sun.position - World.position);

        float sunRayLength = RaySphere(World.position, World.atmosphereRadius, Camera.position, dirToSun).y;
        float sunRayOpticalDepth = OpticalDepth(inScatteredPoint, dirToSun, sunRayLength);
        float viewRayOpticalDepth = OpticalDepth(inScatteredPoint, rayDir, stepSize * i);
        float transmittance = exp(-(sunRayOpticalDepth + viewRayOpticalDepth));
        float localDensity = DensityAtPoint(inScatteredPoint);
        
        inScatteredLight += localDensity * transmittance * stepSize;
        inScatteredPoint += rayDir * stepSize;
    }
    
    return inScatteredLight;
}

float4 main(PixelIn pixelIn) : SV_TARGET
{
    float4 screen = screenTexture.Sample(LinearSampler, pixelIn.TexCoord);
    float depth = depthTexture.Sample(LinearSampler, pixelIn.TexCoord);
    
    float3 viewVector = ScreenToWorld(float3(pixelIn.TexCoord, 0));
    float sceneDepth = LinearEyeDepth(depth) * length(viewVector);
    
    float3 rayDir = normalize(viewVector);

    float2 hitData = RaySphere(World.position, World.atmosphereRadius, Camera.position, rayDir);
    
    float dstToAtmosphere = hitData.x;
    float dstThroughAtmosphere = min(hitData.y, sceneDepth - dstToAtmosphere);
  
    if (dstThroughAtmosphere > 0)
    {
        const float epsilon = 0.0001;
        float3 pointInAtmosphere = Camera.position + rayDir * (dstToAtmosphere + epsilon);
        float3 light = CalculateLight(pointInAtmosphere, rayDir, dstThroughAtmosphere - epsilon * 2);
        return screen * float4(1 - light, 1) + float4(light, 1);
    }
    
    
    
    return screen;
}