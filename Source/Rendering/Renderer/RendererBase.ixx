/*
    Description:
    The interface of RendererBase module

    Created Date:
    5/9/2022 11:38:27 PM

    Notes:
*/

export module RendererBase;

import CommonAbstractionClass;
import RenderingBackend;
import VkRenderingBackend;

using namespace RB;

export class RendererBase
{
public:
    // Initialize Device with given API (Vulkan/DX12/Metal)
    RendererBase(RENDER_API renderAPI = RENDER_API::VULKAN)
    {
        if (renderAPI == RENDER_API::VULKAN)
        {
            RBI = new VkRBInterface();
        }
        else if (renderAPI == RENDER_API::D3D12)
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
    virtual bool Update(float) = 0;
    virtual bool Render() = 0;

protected:
    // Rendering Backend
    RBInterface* RBI;
};