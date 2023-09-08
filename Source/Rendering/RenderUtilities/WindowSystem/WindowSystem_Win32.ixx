/*
    Module Description:
    The interface of Module partition uint WindowSystem:Win32.

    Created Date:
    2022.4.17

	TODO:
	1. I/O Module implementation + Window Procedure process
	2. Cursor Control and lock
	3. borderless fullscreen
*/

export module WindowSystem:Win32;

import :WindowBase;
import Input;
import <stdint.h>;
import <Windows.h>;
import <Vulkan/vulkan.h>;

namespace Window
{

export class Win32Window : public WindowBase
{
public:
	static LRESULT CALLBACK WindowProc(HWND hwnd,UINT uMsg, WPARAM wParam, LPARAM lParam);
public:
	Win32Window(uint32_t width, uint32_t height);

	virtual bool Update(float) const final override;

	static void InitWindowSurface(VkInstance, VkSurfaceKHR&);

	HINSTANCE& GetInstance() { return instance; }
	HWND& GetHWDN() { return windowHandle; }

private:
	bool bContinue = true;
	HINSTANCE instance;
	HWND windowHandle;

	// Global window Instance;
	inline static Win32Window* windowInstance = nullptr;
};

}//namespace Window