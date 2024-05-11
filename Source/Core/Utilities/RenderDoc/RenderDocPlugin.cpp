/*
    Description:
    The implementation of RenderDocPlugin module

    Created Date:
    5/22/2022 6:10:46 PM
*/

module;
#include <windows.h>
#include <assert.h>
#include "renderdoc_app.h"
module RenderDocPlugin;

bool RenderDocPlugin::HookRenderDoc(void* deviceIn, void* wndHandleIn)
{
	// Should be somewhere else
	if (!deviceIn || !wndHandleIn) return false;

	// TODO: Only Used in vulkan
	device = (RENDERDOC_DevicePointer)RENDERDOC_DEVICEPOINTER_FROM_VKINSTANCE(deviceIn);
	wndHandle = (RENDERDOC_WindowHandle)wndHandleIn;

	// RenderDoc Initializing...
	std::wstring renderDocPath;
	if (!QueryRegKey(HKEY_LOCAL_MACHINE, TEXT("SOFTWARE\\Classes\\RenderDoc.RDCCapture.1\\DefaultIcon\\"), TEXT(""), renderDocPath))
	{
		// TODO: Didn't find renderdoc Installed
		return false;
	}


	HMODULE mod = GetModuleHandleA("renderdoc.dll");
	// If renderdoc.dll is not loaded, try to manually do so.
	if (!mod)
	{
		std::string renderDocExe = "\\qrenderdoc.exe"; // Delete the exe file from the search directory
		std::string searchPath = std::string(renderDocPath.cbegin(), renderDocPath.cend() - renderDocExe.size()) + "\\renderdoc.dll"; // Convert wstring to string
		HMODULE mod = LoadLibraryA(searchPath.data());
	}

	if (mod)
	{
		pRENDERDOC_GetAPI RENDERDOC_GetAPI =
			(pRENDERDOC_GetAPI)GetProcAddress(mod, "RENDERDOC_GetAPI");
		int ret = RENDERDOC_GetAPI(eRENDERDOC_API_Version_1_6_0, (void**)&rdoc_api);
		assert(ret == 1);
	}

	if (!rdoc_api)
	{
		throw std::runtime_error("Unable to load renderdoc.dll");
	}
	else
	{
		rdoc_api->SetActiveWindow(device, wndHandle);
		rdoc_api->SetCaptureFilePathTemplate("../../../Resources/RenderDocCapture/Capture");
		RENDERDOC_InputButton keys[] = { RENDERDOC_InputButton::eRENDERDOC_Key_F11 };
		rdoc_api->SetCaptureKeys(keys, 1);
	}

	Input::InputManager::RegisterCallback(Input::Bindings::F11, [this]() { this->TriggerRenderDocCapture(); });

	return true;
}

void RenderDocPlugin::StartRenderDocCapture()
{
	if (rdoc_api) rdoc_api->StartFrameCapture(device, wndHandle);
}

bool RenderDocPlugin::TriggerRenderDocCapture()
{
	if (!rdoc_api) return false;

	//rdoc_api->TriggerCapture();
	if (!rdoc_api->IsTargetControlConnected())
		rdoc_api->LaunchReplayUI(1, nullptr);

	rdoc_api->ShowReplayUI();

	return true;
}

void RenderDocPlugin::EndRenderDocCapture()
{
	if (rdoc_api) rdoc_api->EndFrameCapture(device, wndHandle);
}

bool RenderDocPlugin::QueryRegKey(const HKEY InKey, const TCHAR* InSubKey, const TCHAR* InValueName, std::wstring& OutData)
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