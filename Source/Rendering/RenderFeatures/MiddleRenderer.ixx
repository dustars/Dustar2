/*
    Description:
    The interface of MiddleRenderer module

    Created Date:
    5/9/2022 11:07:35 PM

    Notes:
*/

export module MiddleRenderer;

import RendererBase;
import RenderingBackend;

export class MiddleRenderer : public RendererBase
{
public:
    MiddleRenderer(RB::RENDER_API renderAPI = RB::RENDER_API::VULKAN);

	virtual void Init() final override;
	virtual bool Update() final override;
	virtual bool Render() final override;
private:
};
