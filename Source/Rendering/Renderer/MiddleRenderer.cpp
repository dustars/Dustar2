/*
    Description:
    The interface of MiddleRenderer module

    Created Date:
    5/9/2022 11:07:35 PM

    Notes:
*/

module;

module MiddleRenderer;

import CmdBuffer;
import Pipeline;
import RenderResource;
import RenderResourceManager;
import Model;

using namespace RB;

MiddleRenderer::MiddleRenderer(RENDER_API renderAPI)
    : RendererBase(renderAPI)
	, camera(0, 30, Vector3(1.f, 4.f, -2.f))
{
	camera.SetProjMatrix(Matrix4::Perspective(0.0f, 1000.0f, 800.f / 600.f, 45.0f));
}

void MiddleRenderer::Init()
{
	RenderResourceManager resourceManager(RBI);

	// 资源创建
	// TODO local variable cannot reach the point where Vulkan use it to initialize buffer/image
	// TODO 我显卡居然是256字节对齐？？？？？？
	static std::vector<mat4> matrices;
	mat4 modelMatrix;
	modelMatrix.ToIdentity();
	matrices.push_back(modelMatrix);
	view = camera.BuildViewMatrix();
	matrices.push_back(view);
	matrices.push_back(camera.GetProjMatrix());
	modelMatrix.ToZero();
	matrices.push_back(modelMatrix);
	
	// Model
	Model cube;
	cube.CreateCube();

	ResourceLayout* layout = resourceManager.CreateResourceLayout();
	layout->CreateModelData(cube);
	layout->CreatePushContant("mvp", sizeof(mat4), view.values);
	layout->CreateConstantBuffer("MVPMatrix", sizeof(mat4), sizeof(mat4) * 4, matrices.data());
	//layout->CreateConstantBuffer("testMatrix", sizeof(mat4), sizeof(mat4), modelMatrix.values);

	ShaderFile vert("../Rendering/Shaders/SimpleVertexShader.spv", "main", ShaderType::VS);
	ShaderFile frag("../Rendering/Shaders/SimpleFragmentShader.spv", "mainPS", ShaderType::FS);

	Pipeline& testPipeline = RBI->CreateGraphicsPipeline( layout, ShaderArray{ vert, frag } );

	RBI->AddPass([&testPipeline](CmdBuffer* cmd)
		{
			cmd->Draw(testPipeline);
		}
	);
}

bool MiddleRenderer::Update(float ms)
{
	camera.UpdateCamera(ms);
	view = camera.BuildViewMatrix();
	return RBI->Update(ms);
}

bool MiddleRenderer::Render()
{
    return RBI->Render();
}
