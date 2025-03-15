/*
    Description:
    The implementation of RenderDocPlugin module

    Created Date:
    5/22/2022 6:10:46 PM
*/

module;
#include <assert.h>
#include "renderdoc_app.h"
module RenderDocPlugin;

bool RenderDocPlugin::HookRenderDoc(void* deviceIn, void* wndHandleIn)
{
	// Should be somewhere else
	if (!deviceIn || !wndHandleIn) return false;

	HMODULE mod = GetModuleHandleA("renderdoc.dll");

	// TODO: Only Used in vulkan
	if (mod)
	{
		pRENDERDOC_GetAPI RENDERDOC_GetAPI = (pRENDERDOC_GetAPI)GetProcAddress(mod, "RENDERDOC_GetAPI");
		int ret = RENDERDOC_GetAPI(eRENDERDOC_API_Version_1_6_0, (void**)&rdoc_api);
		assert(ret == 1);
	}
	else
	{
		throw std::runtime_error("renderdoc.dll is not loaded! Please load it before any render API initilization.");
	}

	if (rdoc_api)
	{
		device = (RENDERDOC_DevicePointer)RENDERDOC_DEVICEPOINTER_FROM_VKINSTANCE(deviceIn);
		wndHandle = (RENDERDOC_WindowHandle)wndHandleIn;

		rdoc_api->SetActiveWindow(device, wndHandle);
		rdoc_api->SetCaptureFilePathTemplate("../../../Resources/RenderDocCapture/Capture");
		RENDERDOC_InputButton keys[] = { RENDERDOC_InputButton::eRENDERDOC_Key_F11 };
		rdoc_api->SetCaptureKeys(keys, 1);
	}
	else
	{
		throw std::runtime_error("Unable to retrieve renderdoc API");
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