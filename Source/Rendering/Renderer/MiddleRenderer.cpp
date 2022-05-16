/*
    Description:
    The interface of MiddleRenderer module

    Created Date:
    5/9/2022 11:07:35 PM

    Notes:
*/

module;

module MiddleRenderer;

import Math;
import CmdBuffer;
import Pipeline;
import RenderResource;
import RenderResourceManager;

using namespace RB;

MiddleRenderer::MiddleRenderer(RENDER_API renderAPI)
    : RendererBase(renderAPI)
	, camera(0, 0, Vector3(0.f, 1.f, 0.f))
{
	camera.SetProjMatrix(Matrix4::Perspective(1.0f, 10000.0f, 800.f / 400.f, 45.0f));
}

void MiddleRenderer::Init()
{
	RenderResourceManager resourceManager(RBI);

	ResourceLayout* layout = resourceManager.CreateResourceLayout();
	layout->CreateConstantBuffer();

	mat4 modelMatrix;

	ShaderFile vert("../Rendering/Shaders/SimpleVertexShader.spv", "main", ShaderType::VS);
	ShaderFile frag("../Rendering/Shaders/SimpleFragmentShader.spv", "mainPS", ShaderType::FS);

	Pipeline& testPipeline = RBI->CreateGraphicsPipeline( layout, ShaderArray{ vert, frag } );

	RBI->AddPass([&testPipeline](CmdBuffer* cmd)
		{
			cmd->Draw(testPipeline);
		}
	);
}

bool MiddleRenderer::Update()
{
	return RBI->Update();
}

bool MiddleRenderer::Render()
{
    return RBI->Render();
}
