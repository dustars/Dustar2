//-----------------------------------------------------------------------
// Image Playground for learning Compute Shader and Pipeline in vulkan
//-----------------------------------------------------------------------
[[vk::constant_id(0)]] const uint testA = 1;
[[vk::constant_id(1)]] const uint testB = 1;
[[vk::constant_id(2)]] const uint testC = 1;

// Sampler
[[vk::combinedImageSampler]][[vk::binding(0)]]
SamplerState srcSampler;

// Read Image
[[vk::combinedImageSampler]][[vk::binding(0)]]
Texture2D<float4> srcImage;

[[vk::binding(1)]]
// Saved Image
RWTexture2D<float4> dstImage;

[numthreads(8, 8, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
	float4 color = srcImage.SampleLevel(srcSampler, (float2(id.xy) + 0.5) / float2(128, 128), 0);

    dstImage[id.xy * 2 + uint2(0, 0)] = color;
    dstImage[id.xy * 2 + uint2(0, testA)] = color;
    dstImage[id.xy * 2 + uint2(testB, 0)] = color;
    dstImage[id.xy * 2 + uint2(testC, testC)] = color;
}
