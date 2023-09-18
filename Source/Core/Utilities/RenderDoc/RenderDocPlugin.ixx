/*
    Module Description:
    The module interface and implementation for RenderDoc Plugin

    Author:
    Zhao Han Ning

    Created Date:
    2022.5.3
*/

module;
#include "renderdoc_app.h"
export module RenderDocPlugin;

import <windows.h>;
import std;
import Input;

export class RenderDocPlugin
{
	RENDERDOC_API_1_6_0* rdoc_api = NULL;
	RENDERDOC_DevicePointer device = NULL;
	RENDERDOC_WindowHandle wndHandle = NULL;

public:
	RenderDocPlugin(){}

	bool HookRenderDoc(void*, void*);

	// To start a frame capture, call StartFrameCapture.
	// You can specify NULL, NULL for the device to capture on if you have only one device and
	// either no windows at all or only one window, and it will capture from that device.
	// See the documentation below for a longer explanation
	void StartRenderDocCapture();
	bool TriggerRenderDocCapture();
	void EndRenderDocCapture();

private:
	// 从UE那边抄过来的, 在注册表中找到文件的目录……
	bool QueryRegKey(const HKEY InKey, const TCHAR* InSubKey, const TCHAR* InValueName, std::wstring& OutData);
};










