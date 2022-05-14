/*
    Description:
    The interface of RendererBase module

    Created Date:
    5/9/2022 11:38:27 PM

    Notes:
*/

export module RendererBase;

import RenderingBackend;
import VkRenderingBackend;
import <vector>;

using namespace RB;

export class RendererBase
{
public:
    // Initialize Device with given API (Vulkan/DX12/Metal)
    RendererBase(RB::RENDER_API renderAPI = RB::RENDER_API::VULKAN)
    {
        if (renderAPI == RB::RENDER_API::VULKAN)
        {
            RBI = new VkRBInterface();
        }
        else if (renderAPI == RB::RENDER_API::D3D12)
        {
            // Output Error
        }
        else // Metal
        {
            // Output Error
        }
    }

    ~RendererBase()
    {
        delete RBI;
    }

    virtual void Init() = 0;
    virtual bool Render() = 0;
    virtual bool Update() = 0;

protected:
    // Rendering Backend
    RBInterface* RBI;
};