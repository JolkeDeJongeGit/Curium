// Define input and output structures
struct InputData
{
    float4 color;
};

struct OutputData
{
    float4 invertedColor;
};

// Define the compute shader function
[numthreads(64, 1, 1)]
void main(uint3 dispatchThreadID : SV_DispatchThreadID,
                         RWStructuredBuffer<OutputData> resultBuffer : register(u0),
                         StructuredBuffer<InputData> inputBuffer : register(t0))
{
        // Get the index of the current thread
    uint index = dispatchThreadID.x;

    // Read input color data from the input buffer
    InputData inputData = inputBuffer[index];

    // Invert the colors (subtract each color component from 1.0)
    float4 invertedColor;
    invertedColor.r = 1.0f - inputData.color.r;
    invertedColor.g = 1.0f - inputData.color.g;
    invertedColor.b = 1.0f - inputData.color.b;
    invertedColor.a = inputData.color.a; // Keep alpha unchanged

    // Write the inverted color to the output buffer
    OutputData outputData;
    outputData.invertedColor = invertedColor;
    resultBuffer[index] = outputData;
}