//===========================================================================
// Simple Pixel shader
//===========================================================================

[[vk::combinedImageSampler]][[vk::binding(0)]]
Texture2D<float4> worley;

[[vk::combinedImageSampler]][[vk::binding(0)]]
SamplerState mySampler;

float4 main(float4 InPosition : SV_POSITION, float4 InUV : TEXCOORD0) : SV_Target0
{
    float4 InputColor = worley.Sample(mySampler, InUV.xy);
    //float4 InputColor = float4(InPosition.z, InPosition.z, InPosition.z, 1.f);
    //InputColor = Texture2DSample(displayImage, displayImageSampler, InUV.xy);
    return InputColor;
}