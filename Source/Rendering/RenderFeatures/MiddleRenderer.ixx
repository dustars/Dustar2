/*
    Description:
    The interface of MiddleRenderer module

    Created Date:
    5/9/2022 11:07:35 PM

    Notes:
*/

export module MiddleRenderer;

import RendererBase;

export class MiddleRenderer : public RendererBase
{
public:
	virtual void Init() override;
	virtual void Render() override;
	virtual void Update() override;
private:
    //Device 

};