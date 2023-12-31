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

	//TODO:真的应该在这里吗?
	static void InitWindowSurface(VkInstance, VkSurfaceKHR&);

	HINSTANCE& GetInstance() { return instance; }
	HWND& GetHWDN() { return windowHandle; }

private:
	bool bContinue = true;
	HINSTANCE instance;
	HWND windowHandle;

	// Global window Instance
	// TODO:这里的inline只是为了初始化,不能去掉是因为什么odr-use的backward compatibility, 总之还挺复杂...看了一圈半懂不懂
	// 等以后功力深厚了再看看怎么处理吧
	inline static Win32Window* windowInstance = nullptr;
};

}//namespace Window