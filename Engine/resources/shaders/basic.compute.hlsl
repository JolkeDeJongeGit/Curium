RWTexture2D<float4> g_sharedTexture : register(u0);


uint hash(uint x, uint seed)
{
    const uint m = 0x5bd1e995U;
    uint hash = seed;
    // process input
    uint k = x;
    k *= m;
    k ^= k >> 24;
    k *= m;
    hash *= m;
    hash ^= k;
    // some final mixing
    hash ^= hash >> 13;
    hash *= m;
    hash ^= hash >> 15;
    return hash;
}

// implementation of MurmurHash (https://sites.google.com/site/murmurhash/) for a  
// 2-dimensional unsigned integer input vector.

uint hash(float2 x, uint seed)
{
    const uint m = 0x5bd1e995U;
    uint hash = seed;
    // process first vector element
    uint k = x.x;
    k *= m;
    k ^= k >> 24;
    k *= m;
    hash *= m;
    hash ^= k;
    // process second vector element
    k = x.y;
    k *= m;
    k ^= k >> 24;
    k *= m;
    hash *= m;
    hash ^= k;
	// some final mixing
    hash ^= hash >> 13;
    hash *= m;
    hash ^= hash >> 15;
    return hash;
}


float2 gradientDirection(uint hash)
{
    switch (int(hash) & 3)
    { // look at the last two bits to pick a gradient direction
        case 0:
            return float2(1.0, 1.0);
        case 1:
            return float2(-1.0, 1.0);
        case 2:
            return float2(1.0, -1.0);
        case 3:
            return float2(-1.0, -1.0);
    }
}

float interpolate(float value1, float value2, float value3, float value4, float2 t)
{
    return lerp(lerp(value1, value2, t.x), lerp(value3, value4, t.x), t.y);
}

float2 fade(float2 t)
{
    // 6t^5 - 15t^4 + 10t^3
    return t * t * t * (t * (t * 6.0 - 15.0) + 10.0);
}

float perlinNoise(float2 position, uint seed)
{
    float2 floorPosition = floor(position);
    float2 fractPosition = position - floorPosition;
    uint2 cellCoordinates = uint2(floorPosition);
    float value1 = dot(gradientDirection(hash(cellCoordinates, seed)), fractPosition);
    float value2 = dot(gradientDirection(hash((cellCoordinates + uint2(1, 0)), seed)), fractPosition - float2(1.0, 0.0));
    float value3 = dot(gradientDirection(hash((cellCoordinates + uint2(0, 1)), seed)), fractPosition - float2(0.0, 1.0));
    float value4 = dot(gradientDirection(hash((cellCoordinates + uint2(1, 1)), seed)), fractPosition - float2(1.0, 1.0));
    return interpolate(value1, value2, value3, value4, fade(fractPosition));
}

float perlinNoise(float2 position, int frequency, int octaveCount, float persistence, float lacunarity, uint seed)
{
    float value = 0.0;
    float amplitude = 1.0;
    float currentFrequency = float(frequency);
    uint currentSeed = seed;
    for (int i = 0; i < octaveCount; i++)
    {
        currentSeed = hash(currentSeed, 0x0U); // create a new seed for each octave
        value += perlinNoise(position * currentFrequency, currentSeed) * amplitude;
        amplitude *= persistence;
        currentFrequency *= lacunarity;
    }
    return value;
}


// Define the compute shader function
[numthreads(16, 16, 1)]
void main(uint3 dispatchThreadID : SV_DispatchThreadID)
{
    // Calculate UV coordinates in the range [0, 1]
    float2 uv = dispatchThreadID.xy / float2(2048, 2048);
    uint seed = 0x578437adU;
    
    float value = perlinNoise(uv, 20, 10, 0.5, 2.0, seed); // multiple octaves
    value = (value + 1.0) * 0.5; // convert from range [-1, 1] to range [0, 1]
    // Write the noise value to the output texture
    g_sharedTexture[dispatchThreadID.xy] = value;
}