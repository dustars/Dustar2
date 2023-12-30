/*
    Description:
    The interface of RendererBase module

    Created Date:
    5/9/2022 11:38:27 PM

    Notes:
*/

export module RendererBase;

import Core;
import RenderDocPlugin;
import WindowSystem;
import VkRenderingBackend;
//TODO: 有生之年系列
//import DxRenderingBackend;

namespace RB
{

export class RendererBase
{
public:
    // Initialize Device with given API (Vulkan/DX12/Metal)
    RendererBase(float2 windowSize = float2(1000, 800))
        : window(windowSize.x, windowSize.y)
        , renderDoc()
    {
        //TODO: 并没有对切换图形API的强需求
        RENDER_API renderAPI = RENDER_API::VULKAN;

        if (renderAPI == RENDER_API::VULKAN)
        {
            RBI = new VkRBInterface();
            if(!renderDoc.HookRenderDoc(RB::VkRBInterface::GetVkInstance(), (void*)&window.GetHWDN()))
            {
                throw std::runtime_error("RenderDoc is not hooked under Vulkan API");
            }
        }
        else if (renderAPI == RENDER_API::D3D12)
        {
        }
        else // Metal
        {
        }
    }

    ~RendererBase()
    {
        delete RBI;
    }

    virtual void Init() = 0;
    virtual bool Update(float) = 0;
    virtual bool Render() = 0;

    bool WindowUpdate(float ms) { return window.Update(ms); }

protected:
    // Rendering Backend
    RBInterface* RBI;

    // Window
    Window::Win32Window window;

    RenderDocPlugin renderDoc;
};

}