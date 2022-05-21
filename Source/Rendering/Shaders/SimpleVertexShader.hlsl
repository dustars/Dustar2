//===========================================================================
// Simple Pixel shader
//===========================================================================

struct MVPMatrix
{
    float4x4 model;
    float4x4 view;
    float4x4 proj;
    float4x4 t;
};

struct PushConstants
{
    float4x4 viewMatrix;
};

[[vk::push_constant]] PushConstants cb;

//[[vk::binding(0)]]
ConstantBuffer<MVPMatrix> mvpMatrices;
//[[vk::binding(1)]]
//ConstantBuffer<test> testMatrix;

void main(
    in float4 InPosition : ATTRIBUTE0, in float4 InUV : ATTRIBUTE1,
    out float4 OutPosition : SV_POSITION, out float4 OutUV : TEXCOORD0)
{
    //OutPosition = float4(InPosition.x * 2.0 - 1.0, 1.0 - 2.0 * InPosition.y, 0, 1); // 1 - y是为了flip Y轴？
    float4x4 mvp = /*mvpMatrices.proj * mvpMatrices.view **/ cb.viewMatrix * mvpMatrices.model; // * testMatrix.t;
    InPosition.y = -InPosition.y; // HLSL 和 Spriv 似乎 y 轴是反的……- -
    OutPosition = mul(float4(InPosition), mvp);
    OutUV.xy = InUV.xy;
}