//===========================================================================
// Simple Pixel shader
//===========================================================================

float4 main(float4 InPosition : SV_POSITION, float4 InUV : TEXCOORD0) : SV_Target0
{
    float4 InputColor = float4(InUV.x, InUV.y, InUV.z, 1.f);
    //float4 InputColor = float4(InPosition.z, InPosition.z, InPosition.z, 1.f);
    //InputColor = Texture2DSample(displayImage, displayImageSampler, InUV.xy);
    return InputColor;
}