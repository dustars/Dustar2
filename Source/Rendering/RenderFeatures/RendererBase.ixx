/*
    Description:
    The interface of RendererBase module

    Created Date:
    5/9/2022 11:38:27 PM

    Notes:
*/

export module RendererBase;

import Device;
import Pipeline;
import <vector>;

using namespace RB;

export class RendererBase
{
    // 考考自己：是否可以不暴露渲染后端Device的情况下，在Derived Renderer class里完成渲染的构造？
	Device device;
	std::vector<Pipeline> pipelines;
public:
    // Initialize Device with given API (Vulkan/DX12/Metal)
    RendererBase();

    virtual void Init() = 0;
    virtual void Render() = 0;
    virtual void Update() = 0;
};