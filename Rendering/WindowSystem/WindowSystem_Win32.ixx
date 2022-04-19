export module WindowSystem:Win32;

import Core;
import <Windows.h>;
import :WindowBase;

namespace Window
{

export class Win32Window : public WindowBase
{
public:
	Win32Window(uint width, uint height);
	//void InitializeWindow() override;
	bool Update()			const override;

private:
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	HINSTANCE instance;
	HWND window;
};

}//namespace Window