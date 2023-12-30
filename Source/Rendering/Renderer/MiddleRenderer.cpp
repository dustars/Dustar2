/*
    Description:
    The interface of MiddleRenderer module

    Created Date:
    5/9/2022 11:07:35 PM

    Notes:
	1. 关于矩阵很重要的记录:
		目前CPU这边是column-major，也就是倒数四三二存储的是位移
		GPU Shader HLSL那边，需要这么用：Mul(矩阵, Position)（HLSL会被翻译为SPIR-V，这期间会不会转换需要查一下）
		Vulkan内部NDC空间，Z是0到1，OpenGL是-1到1，所以PS输出的Z如果不在范围0-1，就看不见鸟

		什么叫Column-major/row-major:
		先说个例子：对于一个3x4的矩阵，一般数学意义上我们会这么写：
		[ 1,  2,  3,  4 ]
		[ 5,  6 , 7,  8 ]
		[ 9, 10, 11, 12 ]
		即，数字3代表行，数字4代表列，一共3x4=12个元素
		
		但在计算机上，数据会需要连续的存储，直观上来看，上面这个矩阵如果是计算机中存储的数据，那
		就应该是 1,2,3,4,5,6,7,8,9,10,11,12 这样连续的数组，当然，有时为了方便的访问第几行
		第几列，我们会把这串数据解释为二维数组，那就变成了 [[1,2,3,4],[5,6,7,8],[9,10,11,12]]
		
		这种最符合我们数学习惯的矩阵存储方式，是以一行一行为单位的，那自然就叫 Row-major
		M[i][j], i访问的是第几行，j访问的是第几列，比如M[1][0]，就是数字5

		我真的希望一切到此为止，一切就该这么看待，但可惜，上述方式虽然直观，但在计算机图形学看来，却并不性能友好。
		说到这里，你可能也猜到了，性能友好的是Column-major，即以一列一列为单位的矩阵存储方式。

		好，还是拿最上面的矩阵来说，如果此时连续存储的是列，那本来从左到右，从上到下很自然的排列方式(row-major)，
		就成了从上到下，从左到右(column-major)，这意味着什么？

		意味着，连续的数字变成了 1,5,9,2,6,10,3,7,11,4,8,12, 解释为二维数组，那就是 [[1,5,9],[2,6,10],[3,7,11],[4,8,12]]
		M[i][j], i是列，j是行，要访问数字5，那就是M[0][1]……

		---------------------------------------
		麻烦的是，有时候我们用到的不同系统，会采用不同的major，即系统A是row-major，系统B又是column-major，系统C又是row
		矩阵数据在不同系统间要保持统一，那就需要各种转换，大脑也需要去记忆这些额外的不必要的开销，就很SB。

		---------------------------------------
		目前的情况:
		CPU这边是Colunm-major，直接用一个大小为16的数组存储的。
		HLSL这边是Row-major，所以CPU这边提供的数据就Tranpose了
		虽然SPIR-V变回了Column-major，但是...

		由于我们写的是HLSL，所以Shader和C++这边的矩阵操作是相反的，就很SB
*/

module MiddleRenderer;

import std;
import Core;
import CmdBuffer;
import Pipeline;
import RenderResource;
import RenderResourceManager;

using namespace RB;

MiddleRenderer::MiddleRenderer(float2 windowSize)
    : RendererBase(windowSize)
{}

void MiddleRenderer::Init()
{
	//Check
	if (camera == nullptr)
	{
		throw std::runtime_error("MiddleRenderer: Camera is not set up!");
	}

	constexpr float	PI_OVER_360 = 3.14159265358979323846f / 360.0f;

	RenderResourceManager resourceManager(RBI);

	// 资源创建
	// TODO local variable cannot reach the point where Vulkan use it to initialize buffer/image
	// TODO 我显卡居然是256字节对齐？？？？？？
	static std::vector<mat4> matrices;

	mat4 modMatrix = mat4::Rotation(45.f, float3(0, 1, 0)) * mat4::Scale(float3(1, 1, 1));

	// Uniform Buffer Matrices
	matrices.push_back(modMatrix);
	matrices.push_back(camera->viewMatrix);
	matrices.push_back(camera->orthMatrix);
	matrices.push_back(camera->projMatrix); //Dumpy matrix, no actual purpose, just for 256 bytes alignment

	// Model
	Mesh m;
	m.CreateCube();
	//m.CreateTriangle();

	ResourceLayout* layout = resourceManager.CreateResourceLayout();
	layout->CreateMeshData(m);
	layout->CreatePushContant("mvp", sizeof(mat4), camera->viewMatrix.values);
	//layout->CreatePushContant("mvp", sizeof(mat4), &projGLM);
	layout->CreateConstantBuffer("MVPMatrix", sizeof(mat4), sizeof(mat4) * 4, matrices.data());
	//layout->CreateConstantBuffer("testMatrix", sizeof(mat4), sizeof(mat4), modelMatrix.values);


	//TODO: https://registry.khronos.org/vulkan/site/guide/latest/hlsl.html
	// 把Shader编译整合进代码来

	ShaderFile vert("../Rendering/Shaders/SimpleVertexShader.spv", "main", ShaderType::VS);
	ShaderFile frag("../Rendering/Shaders/SimpleFragmentShader.spv", "main", ShaderType::FS);

	Pipeline& testPipeline = RBI->CreateGraphicsPipeline( layout, ShaderArray{ vert, frag } );

	RBI->AddPass([&testPipeline](CmdBuffer* cmd)
		{
			cmd->Draw(testPipeline);
		}
	);
}

bool MiddleRenderer::Update(float ms)
{
	return RBI->Update(ms);
}

bool MiddleRenderer::Render()
{
    return RBI->Render();
}
