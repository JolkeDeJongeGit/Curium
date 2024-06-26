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
    float4 scatteringCoefficients;
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



float2 RaySphere(float3 sphereCentre, float sphereRadius, float3 rayOrigin, float3 rayDir)
{
    float3 offset = rayOrigin - sphereCentre;
    float a = 1; // Set to dot(rayDir, rayDir) if rayDir might not be normalized
    float b = 2 * dot(offset, rayDir);
    float c = dot(offset, offset) - sphereRadius * sphereRadius;
    float d = b * b - 4 * a * c; // Discriminant from quadratic formula

		// Number of intersections: 0 when d < 0; 1 when d = 0; 2 when d > 0
    if (d > 0)
    {
        float s = sqrt(d);
        float dstToSphereNear = max(0, (-b - s) / (2 * a));
        float dstToSphereFar = (-b + s) / (2 * a);

			// Ignore intersections that occur behind the ray
        if (dstToSphereFar >= 0)
        {
            return float2(dstToSphereNear, dstToSphereFar - dstToSphereNear);
        }
    }
		// Ray did not intersect sphere
    return float2(0, 0);
}
float3 ScreenToWorld(float2 pos)
{
    float4 posP = float4(pos.x * 2.0 - 1.0, (1. - pos.y) * 2.0f - 1.0f, 0, 1.0); // Don't forget to invert Y.
    float3 posVS = mul(Camera.inverseProjectionMatrix, posP); // Clip to view space
    float4 posWS = mul(Camera.viewMatrix, float4(posVS, 0)); // View to world space
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

float OpticalDepth2(float3 rayOrigin, float3 rayDir, float rayLength)
{
    float3 endPoint = rayOrigin + rayDir * rayLength;
    float d = dot(rayDir, normalize(rayOrigin - World.position));
    float opticalDepth = 0;

    const float blendStrength = 1.5;
    float w = saturate(d * blendStrength + .5);
				
    float d1 = OpticalDepth(rayOrigin, rayDir, rayLength) - OpticalDepth(endPoint, rayDir, rayLength);
    float d2 = OpticalDepth(endPoint, -rayDir, rayLength) - OpticalDepth(rayOrigin, -rayDir, rayLength);

    opticalDepth = lerp(d2, d1, w);
    return opticalDepth;
}

float CalculateLight(float3 rayOrigin, float3 rayDir, float rayLength, float4 originalCol)
{
    float3 inScatteredPoint = rayOrigin;
    float stepSize = rayLength / (NUM_SCATTERING_POINTS - 1);
    float inScatteredLight = 0;
    float viewRayOpticalDepth = 0;
    float intensity = 1.f;
    float3 dirToSun = normalize(Sun.position - World.position);
    for (int i = 0; i < NUM_SCATTERING_POINTS; i++)
    {

        float sunRayLength = RaySphere(World.position, World.atmosphereRadius, Camera.position, dirToSun).y;
        float sunRayOpticalDepth = OpticalDepth(inScatteredPoint + dirToSun * 0.8f, dirToSun, sunRayLength);
        float localDensity = DensityAtPoint(inScatteredPoint);
        viewRayOpticalDepth = OpticalDepth2(inScatteredPoint, rayDir, stepSize * i);
        float transmittance = exp(-(sunRayOpticalDepth + viewRayOpticalDepth) * World.scatteringCoefficients);
        
        inScatteredLight += localDensity * transmittance;
        inScatteredPoint += rayDir * stepSize;
    }
    
    inScatteredLight *= World.scatteringCoefficients * intensity * stepSize / World.planetRadius;

	// Attenuate brightness of original col (i.e light reflected from planet surfaces)
	// This is a hacky mess, TODO: figure out a proper way to do this
    const float brightnessAdaptionStrength = 0.15;
    const float reflectedLightOutScatterStrength = 3;
    float brightnessAdaption = dot(inScatteredLight, 1) * brightnessAdaptionStrength;
    float brightnessSum = viewRayOpticalDepth * intensity * reflectedLightOutScatterStrength + brightnessAdaption;
    float reflectedLightStrength = exp(-brightnessSum);
    float hdrStrength = saturate(dot(originalCol, 1) / 3 - 1);
    reflectedLightStrength = lerp(reflectedLightStrength, 1, hdrStrength);
    float3 reflectedLight = originalCol * reflectedLightStrength;

    float3 finalCol = reflectedLight + inScatteredLight;

				
    return finalCol;
    
    return inScatteredLight;
}

float4 main(PixelIn pixelIn) : SV_TARGET
{
    float4 screen = screenTexture.Sample(LinearSampler, pixelIn.TexCoord);
    float depth = depthTexture.Sample(LinearSampler, pixelIn.TexCoord);
    
    float3 viewVector = ScreenToWorld(pixelIn.TexCoord);
    float sceneDepth = LinearEyeDepth(depth) * length(viewVector);
    
    float3 rayDir = normalize(viewVector);

    float2 hitData = RaySphere(World.position, World.atmosphereRadius, Camera.position, rayDir);
    
    float dstToAtmosphere = hitData.x ;
    return float4(viewVector, 1);
    //return hitData.x;
    float dstThroughAtmosphere = min(hitData.y, sceneDepth - dstToAtmosphere);
    //return dstThroughAtmosphere / (World.atmosphereRadius * 2);
    if (dstThroughAtmosphere > 0)
    {
        const float epsilon = 0.0001;
        float3 pointInAtmosphere = Camera.position + rayDir * (dstToAtmosphere + epsilon);
        float3 light = CalculateLight(pointInAtmosphere, rayDir, dstThroughAtmosphere - epsilon * 2, screen);
        return float4(light, 1);
    }

    return screen;
}