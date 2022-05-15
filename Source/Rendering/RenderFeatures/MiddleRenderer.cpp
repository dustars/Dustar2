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

using namespace RB;

MiddleRenderer::MiddleRenderer(RENDER_API renderAPI)
    : RendererBase(renderAPI)
{
}

void MiddleRenderer::Init()
{
	// 配置资源
    // RBI->CreateVertexBuffer();
    // RBI->CreateSRV();
    // RBI->CreateUAV();
    // RBI->CreateConstant();

	ShaderFile vert("../Rendering/Shaders/SimpleVertexShader.spv", "main", ShaderType::VS);
	ShaderFile frag("../Rendering/Shaders/SimpleFragmentShader.spv", "mainPS", ShaderType::FS);

	Pipeline& testPipeline = RBI->CreateGraphicsPipeline( ShaderArray{ vert, frag } );

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
