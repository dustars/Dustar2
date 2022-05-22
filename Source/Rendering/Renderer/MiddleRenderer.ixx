/*
    Description:
    The interface of MiddleRenderer module

    Created Date:
    5/9/2022 11:07:35 PM

    Notes:
*/
export module MiddleRenderer;

import CommonAbstractionClass;
import RendererBase;
import RenderingBackend;
import Camera;
import Math;

export class MiddleRenderer : public RendererBase
{
public:
    MiddleRenderer(RENDER_API renderAPI = RENDER_API::VULKAN);

	virtual void Init() final override;
	virtual bool Update(float) final override;
	virtual bool Render() final override;
private:
    Camera camera;
    mat4    model;
    mat4    view;
};
