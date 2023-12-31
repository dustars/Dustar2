/*
    Description:
    The interface of MiddleRenderer module

    Created Date:
    5/9/2022 11:07:35 PM

    Notes:
*/
export module MiddleRenderer;

import Core;
import RendererBase;

import Camera;

export class MiddleRenderer : public RB::RendererBase
{
public:
    MiddleRenderer(void* wndHandleIn = nullptr)
        : RendererBase(wndHandleIn)
    {}

	virtual void Init() final override;
	virtual bool Update(double) final override;
	virtual bool Render() final override;

    //Camera data needs to be set before rendering!
    void SetCameraRenderData(std::shared_ptr<CameraRenderData> cameraData) { camera = cameraData; }

private:
    std::shared_ptr<CameraRenderData> camera;
};
