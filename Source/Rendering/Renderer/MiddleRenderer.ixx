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

export class MiddleRenderer : public RB::RendererBase
{
public:
    MiddleRenderer(float2 windowSize = float2(1000, 800));

	virtual void Init() final override;
	virtual bool Update(float) final override;
	virtual bool Render() final override;
};
