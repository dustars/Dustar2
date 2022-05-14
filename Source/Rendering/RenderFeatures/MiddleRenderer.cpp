/*
    Description:
    The interface of MiddleRenderer module

    Created Date:
    5/9/2022 11:07:35 PM

    Notes:
*/

module;

module MiddleRenderer;

MiddleRenderer::MiddleRenderer(RB::RENDER_API renderAPI)
    : RendererBase(renderAPI)
{
}

void MiddleRenderer::Init()
{
    // 配置资源
    // RBI->CreateVertexBuffer();
    // 
    // 创建管线
	// RBI->CreateGraphicsPipeline();
    // 
    // 添加Pass
	/*RBI->CreateGraphicsPass(
	[vertex, pipeline ? ? ? ](CommandBuffer cmd)
	{
		//绑定资源
		cmd->BindVertexBuffer(vertex);
		//绑定Pipeline
		cmd->BindPipeline(pipeline)
			//Draw
			cmd->Draw();
	});
    
    */
}

bool MiddleRenderer::Update()
{
	return RBI->Update();
}

bool MiddleRenderer::Render()
{
    return RBI->Render();
}
