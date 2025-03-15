/*
    Description:
    The interface of RendererBase module

    Created Date:
    5/9/2022 11:38:27 PM

    Notes:
*/

#define ENABLE_RENDERDOC 1

module;
#ifdef ENABLE_RENDERDOC
#include <windows.h>
#include <stdint.h>
#endif

export module RendererBase;

import Core;
import RenderDocPlugin;
import WindowSystem;
import VkRenderingBackend;
//TODO: 有生之年系列
//import DxRenderingBackend;

bool InjectRenderDocDLL();

namespace RB
{

export class RendererBase
{
public:
    RendererBase()
    {
#ifdef ENABLE_RENDERDOC
		// Renderdoc.dll must be injected before API Initialization
		InjectRenderDocDLL();
#endif
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
#ifdef ENABLE_RENDERDOC
        if (wndHandleIn)
        {
            if (!renderDoc.HookRenderDoc(RB::VkRBInterface::GetVkInstance(), wndHandleIn))
            {
                throw std::runtime_error("RenderDoc is not hooked under Vulkan API");
            }
        }
#endif
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

#ifdef ENABLE_RENDERDOC

// Renderdoc
bool QueryRegKey(const HKEY InKey, const TCHAR* InSubKey, const TCHAR* InValueName, std::wstring& OutData)
{
    bool bSuccess = false;

    // Redirect key depending on system
    for (int32_t RegistryIndex = 0; RegistryIndex < 2 && !bSuccess; ++RegistryIndex)
    {
        HKEY Key = 0;
        const uint32_t RegFlags = (RegistryIndex == 0) ? KEY_WOW64_32KEY : KEY_WOW64_64KEY;
        if (RegOpenKeyEx(InKey, InSubKey, 0, KEY_READ | RegFlags, &Key) == ERROR_SUCCESS)
        {
            ::DWORD Size = 0;
            // First, we'll call RegQueryValueEx to find out how large of a buffer we need
            if ((RegQueryValueEx(Key, InValueName, NULL, NULL, NULL, &Size) == ERROR_SUCCESS) && Size)
            {
                // Allocate a buffer to hold the value and call the function again to get the data
                char* Buffer = new char[Size];
                if (RegQueryValueEx(Key, InValueName, NULL, NULL, (LPBYTE)Buffer, &Size) == ERROR_SUCCESS)
                {
                    // Convert CHAR to WCHAR by using NormalizeString(). Converted Data is contained in a wstring.
                    auto Length = NormalizeString(NormalizationC, (LPCWSTR)Buffer, -1, NULL, 0);
                    LPWSTR str = new WCHAR[Length];
                    NormalizeString(NormalizationC, (LPCWSTR)Buffer, -1, str, Length);
                    OutData = std::wstring(str);
                    delete[] str;
                    bSuccess = true;
                }
                delete[] Buffer;
            }
            RegCloseKey(Key);
        }
    }

    return bSuccess;
}

bool InjectRenderDocDLL()
{
    std::wstring renderDocPath;
    if (!QueryRegKey(HKEY_LOCAL_MACHINE, TEXT("SOFTWARE\\Classes\\RenderDoc.RDCCapture.1\\DefaultIcon\\"), TEXT(""), renderDocPath))
    {
        // TODO: Didn't find renderdoc Installed
        return false;
    }

    std::string renderDocExe = "\\qrenderdoc.exe"; // Delete the exe file from the search directory
    std::string searchPath = std::string(renderDocPath.cbegin(), renderDocPath.cend() - renderDocExe.size()) + "\\renderdoc.dll"; // Convert wstring to string
    HMODULE mod = LoadLibraryA(searchPath.data());
    return mod;
}

#endif