//===========================================================================
// Vertex shader for testing buffer address feature
//===========================================================================

// RawBufferLoad currently dont care the memory layout of what being accessed
// So I've hardcoded the offset and the size of vertex, which is totally not ok
// TODO: How to solve it?
// An interesting proposal to improve this feature: 
// https://microsoft.github.io/hlsl-specs/proposals/0010-vk-buffer-ref.html
// 蛋疼的还是hlsl翻译成spir-v, 教程上glsl翻译根本没啥问题, 我到底要不要继续搞hlsl?
// 之后一定会大量踩坑的啊....
// struct Vertex
// {
//     float4 pos;
//     float4 uv;
// };

struct MVPMatrix
{
    float4x4 model;
    float4x4 view;
    float4x4 perspectiveProj;
    float4x4 placeholder;
};

struct PushConstants
{
    uint64_t vertexBufferAddresss;
};

[[vk::push_constant]] PushConstants vertexBuffer;

[[vk::binding(0)]]
ConstantBuffer<MVPMatrix> mvpMatrices;

void main(
    in uint index : SV_VertexID,
    out float4 OutPosition : SV_POSITION, out float4 OutUV : TEXCOORD0)
{
    float4 position = vk::RawBufferLoad<float4>(vertexBuffer.vertexBufferAddresss + index * 32, 16);
    float4 uv = vk::RawBufferLoad<float4>(vertexBuffer.vertexBufferAddresss + index * 32 + 16, 16);
    
    OutPosition = mul(mvpMatrices.perspectiveProj, mul(mvpMatrices.view, mul(mvpMatrices.model, position)));
    OutUV = uv; // Note that w are empty
}