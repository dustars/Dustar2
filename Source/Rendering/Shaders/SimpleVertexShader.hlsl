//===========================================================================
// Simple Vertex shader
//===========================================================================

struct MVPMatrix
{
    float4x4 model;
    float4x4 view;
    float4x4 proj;
    float4x4 t;
};

// From: https://github.com/Microsoft/DirectXShaderCompiler/blob/main/docs/SPIR-V.rst#hlsl-functions
// Applying row_major or column_major attributes to a stand-alone matrix will be ignored
// by the compiler because RowMajor and ColMajor decorationsin SPIR-V are only allowed
// to be applied to members of structures. A warning will be issued by the compiler.
struct PushConstants
{
    float4x4 view;
};

[[vk::push_constant]] PushConstants cb;

[[vk::binding(0)]]
ConstantBuffer<MVPMatrix> mvpMatrices;

//[[vk::binding(1)]]
//ConstantBuffer<test> testMatrix;

void main(
    in float4 InPosition : ATTRIBUTE0, in float4 InUV : ATTRIBUTE1,
    out float4 OutPosition : SV_POSITION, out float4 OutUV : TEXCOORD0)
{
    //OutPosition = float4(InPosition.x * 2.0 - 1.0, 1.0 - 2.0 * InPosition.y, 0, 1); // 1 - y是为了flip Y轴？
    float4x4 mvp = mvpMatrices.proj * cb.view * mvpMatrices.model;
    OutPosition = mul(InPosition, mvp); // For column-major
    //OutPosition = mul(mvpMatrices.model, float4(InPosition)); // For row-major
    OutUV.xy = InUV.xy;
}