export module WindowSystem:Win32;

import <stdint.h>;
import <Windows.h>;
import :WindowBase;

namespace Window
{

export class Win32Window : public WindowBase
{
public:
	Win32Window(uint32_t width, uint32_t height);
	//void InitializeWindow() override;
	bool Update()			const override;

	HINSTANCE GetInstance() const { return instance; }
	HWND GetHWDN() const { return windowHandle; }

private:
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	HINSTANCE instance;
	HWND windowHandle;
};

}//namespace Window