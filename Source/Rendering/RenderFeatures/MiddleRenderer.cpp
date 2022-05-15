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

MiddleRenderer::MiddleRenderer(RB::RENDER_API renderAPI)
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

    RB::Pipeline& testPipeline = RBI->CreateGraphicsPipeline();

	RBI->AddPass([&testPipeline](RB::CmdBuffer* cmd)
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
