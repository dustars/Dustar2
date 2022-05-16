//===========================================================================
// Simple Pixel shader
//===========================================================================

struct MVPMatrix
{
    float4x4 model;
    float4x4 view;
    float4x4 proj;
};

ConstantBuffer<MVPMatrix> mvpMatrices;

void main(
    in float4 InPosition : ATTRIBUTE0, in float2 InUV : ATTRIBUTE1,
    out float4 OutPosition : SV_POSITION, out float2 OutUV : TEXCOORD0)
{
    //OutPosition = float4(InPosition.x * 2.0 - 1.0, 1.0 - 2.0 * InPosition.y, 0, 1); // 1 - y是为了flip Y轴？
    float4x4 mvp = mvpMatrices.proj * mvpMatrices.view * mvpMatrices.model;
    OutPosition = mul(float4(InPosition), mvp);
    OutUV = InUV;
}