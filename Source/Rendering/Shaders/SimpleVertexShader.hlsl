//===========================================================================
// Simple Vertex shader
//===========================================================================

struct MVPMatrix
{
    float4x4 model;
    float4x4 view;
    float4x4 orthogonalProj;
    float4x4 perspectiveProj;
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

[[vk::binding(1)]]
ConstantBuffer<MVPMatrix> mvpMatrices;

//[[vk::binding(1)]]
//ConstantBuffer<test> testMatrix;

void main(
    in float4 InPosition : ATTRIBUTE0, in float4 InUV : ATTRIBUTE1,
    out float4 OutPosition : SV_POSITION, out float4 OutUV : TEXCOORD0)
{
    //下面为错误示范, 不要用这种方式算矩阵, 编译成SPIR-V的时候不会自动Tranpose
    //float4x4 mvp = mvpMatrices.model * mvpMatrices.view * mvpMatrices.proj;
    //OutPosition = mul(mvp, InPosition);
    //float4x4 mvp = mvpMatrices.MVP;
    
    OutPosition = mul(mvpMatrices.perspectiveProj, mul(cb.view, mul(mvpMatrices.model, InPosition)));
    OutUV = InUV;
}