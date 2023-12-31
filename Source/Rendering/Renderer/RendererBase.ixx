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
    RendererBase()
    {
        // Initialize Device with given API (Vulkan/DX12/Metal)
        //TODO: Move to configuration file
        RENDER_API renderAPI = RENDER_API::VULKAN;

        if (renderAPI == RENDER_API::VULKAN)
        {
            RBI = new VkRBInterface();
        }
        else if (renderAPI == RENDER_API::D3D12)
        {
        }
        else // Metal if it ever existed...
        {
        }
    }

    // Delegating constructor to initialize renderDoc
    RendererBase(void* wndHandleIn) : RendererBase()
    {
        if (wndHandleIn)
        {
            if (!renderDoc.HookRenderDoc(RB::VkRBInterface::GetVkInstance(), wndHandleIn))
            {
                throw std::runtime_error("RenderDoc is not hooked under Vulkan API");
            }
        }
    }

    ~RendererBase()
    {
        delete RBI;
    }

    virtual void Init() = 0;
    //TODO: Should renderer be responsible to update some data?
    virtual bool Update(double) = 0;
    virtual bool Render() = 0;

protected:
    // Rendering Backend
    RBInterface* RBI;

    RenderDocPlugin renderDoc;
};

}