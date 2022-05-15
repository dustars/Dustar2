//===========================================================================
// Simple Pixel shader
//===========================================================================

float4 mainPS(float4 InPosition : SV_POSITION, float2 InUV : TEXCOORD0) : SV_Target0
{
    float4 InputColor = float4(InUV.x, InUV.y, 0.f, 1.f);
    //InputColor = Texture2DSample(displayImage, displayImageSampler, InUV.xy);
    return InputColor;
}