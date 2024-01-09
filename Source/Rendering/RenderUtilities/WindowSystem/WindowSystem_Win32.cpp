/*
    Module Description:
    The implementation of Module partition uint WindowSystem Win32.

    Created Date:
    2022.4.17
*/

module;
#include <windowsx.h>
#include <strsafe.h>
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan\vulkan.h>
module WindowSystem:Win32;

import std;

namespace Window
{

Win32Window::Win32Window(uint32_t width, uint32_t height)
{
    constexpr wchar_t CLASS_NAME[]  = L"Window Class";

	// Register the window class.
	WNDCLASS wc = { };

	wc.lpfnWndProc   = (WNDPROC)WindowProc;
	wc.hInstance     = GetModuleHandle(NULL);
	wc.lpszClassName = CLASS_NAME;

    if(!RegisterClass(&wc)) {
        throw std::runtime_error("Win32Window::Win32Window(): Failed to register class!");
        return;
    }

    // Note: These two functions return the *Primary Display* resolution (in pixels)
    int centerX = GetSystemMetrics(SM_CXSCREEN)/2;
    int centerY = GetSystemMetrics(SM_CYSCREEN)/2;

    // This is where the window is actually created and returned by OS as a handle
	windowHandle = CreateWindowEx(
        0,                              // Optional window styles.
        CLASS_NAME,                     // Window class
        L"Dustar2",                // Window text
        WS_OVERLAPPEDWINDOW|WS_VISIBLE,            // Window style

        centerX - width/2, centerY - height/2,      //Window position
        (int)width, (int)height,                    //Window size

        NULL,       // Parent window    
        NULL,       // Menu
        GetModuleHandle(NULL),   // Instance handle
        this        // Additional application data
    );

    if(!windowHandle) throw std::runtime_error("Cannot Create Window");
    windowInstance = this;

    //Input
    RAWINPUTDEVICE Rid[2];

    Rid[0].usUsagePage = 0x01;          // HID_USAGE_PAGE_GENERIC
    Rid[0].usUsage = 0x02;              // HID_USAGE_GENERIC_MOUSE
    Rid[0].dwFlags = RIDEV_NOLEGACY;    // adds mouse and also ignores legacy mouse messages
    Rid[0].hwndTarget = 0;

    Rid[1].usUsagePage = 0x01;          // HID_USAGE_PAGE_GENERIC
    Rid[1].usUsage = 0x06;              // HID_USAGE_GENERIC_KEYBOARD
    Rid[1].dwFlags = RIDEV_NOLEGACY;    // adds keyboard and also ignores legacy keyboard messages
    Rid[1].hwndTarget = 0;

    if (RegisterRawInputDevices(Rid, 2, sizeof(Rid[0])) == FALSE)
    {
        //registration failed. Call GetLastError for the cause of the error
    }
}

//TODO:真的应该在这个地方吗?
void Win32Window::InitWindowSurface(VkInstance vkInstance, VkSurfaceKHR& surface)
{
	VkWin32SurfaceCreateInfoKHR createInfo;
	createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	createInfo.pNext = nullptr;
	createInfo.flags = 0;
	createInfo.hinstance = GetModuleHandle(NULL);
	createInfo.hwnd = windowInstance->GetHWDN();

	if (VK_SUCCESS != vkCreateWin32SurfaceKHR(vkInstance, &createInfo, nullptr, &surface))
	{
		throw std::runtime_error("Failed to create Win32 Surface");
	}

}

bool Win32Window::Update(float ms) const
{
	std::wstringstream wss;
    wss << L"Dustars2   FPS:" << int(1000.f/ms);
    SetWindowText(windowHandle, wss.str().c_str());

    MSG msg{};
    if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return bContinue;
}

// TODO: 对于不少Messages的处理最好是开一个thread，然后主线程继续执行，不然整个渲染就卡住了……
LRESULT Win32Window::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    wchar_t msg[32];
    switch (uMsg)
    {
        case WM_CREATE:
		{
			CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
			Win32Window* pThis = (Win32Window*)pCreate->lpCreateParams;
            SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pThis);
			
            return 0;
		}
        //TODO:把鼠标按键和滚轮设置了
        case WM_INPUT:
        {
            UINT dwSize = sizeof(RAWINPUT);
            static BYTE lpb[sizeof(RAWINPUT)];

            GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER));

            RAWINPUT* raw = (RAWINPUT*)lpb;

            if (raw->header.dwType == RIM_TYPEKEYBOARD)
            {
                switch (raw->data.keyboard.Flags)
                {
                    case RI_KEY_MAKE:   // Key is pressed
                    {
                        Input::InputManager::UpdateBindingWindowsRaw(uint32_t(raw->data.keyboard.MakeCode), true);
                        break;
                    }
                    case RI_KEY_BREAK:  // Key is released
                    {
                        Input::InputManager::UpdateBindingWindowsRaw(uint32_t(raw->data.keyboard.MakeCode), false);
                        break;
                    }
                    default:
                    {
                        //还有啥 RI_KEY_E0/RI_KEY_E1/RI_KEY_MAKE
                        //TODO:目前没用途, 无视, 后续考虑给个信息之类的
                        break;
                    }
                }
            }
            else if (raw->header.dwType == RIM_TYPEMOUSE)
            {
                int relativeX = raw->data.mouse.lLastX;
                int relativeY = raw->data.mouse.lLastY;
                Input::InputManager::UpdateCursorByLastFrameMoveOffset(relativeX, relativeY);
            }

            return 0;
        }

        //The following code processes legacy windows mouse/keyboard input,
        //which is not needed because it failed to suffice for locking
        //mouse at the center of the window while also moving the camera
/*
        case WM_MOUSEMOVE:
        {
			int xPos = GET_X_LPARAM(lParam);
			int yPos = GET_Y_LPARAM(lParam);
            Input::InputManager::UpdateMouse(float(xPos), float(yPos));
            // TODO: 可能还是要使用Raw Input API来完美处理输入
            //SetCursorPos(back.x, back.y);

            return 0;
        }
        case WM_LBUTTONDOWN:
            Input::InputManager::UpdateBindingWindows(uint32_t(wParam), true);
            return 0;
		case WM_RBUTTONDOWN:
			Input::InputManager::UpdateBindingWindows(uint32_t(wParam), true);
			return 0;
		case WM_MBUTTONDOWN:
			Input::InputManager::UpdateBindingWindows(uint32_t(wParam), true);
			return 0;
		case WM_LBUTTONUP:
            Input::InputManager::UpdateBindingWindows(uint32_t(wParam), false);
            return 0;
        case WM_RBUTTONUP:
            Input::InputManager::UpdateBindingWindows(uint32_t(wParam), false);
            return 0;
        case WM_MBUTTONUP:
            Input::InputManager::UpdateBindingWindows(uint32_t(wParam), false);
            return 0;
		case WM_XBUTTONDOWN:
		{
            Input::InputManager::UpdateBindingWindows(uint32_t(GET_XBUTTON_WPARAM(wParam)), true);
			return 0;
		}
		case WM_XBUTTONUP:
		{
			Input::InputManager::UpdateBindingWindows(uint32_t(GET_XBUTTON_WPARAM(wParam)), false);
			return 0;
		}
        case WM_MOUSEWHEEL:
        {
            // uints of mouse wheel movement
            Input::InputManager::UpdateMouse(GET_WHEEL_DELTA_WPARAM(wParam));
        }
        // 目前不处理 system key
        case WM_KEYDOWN:
        {
            Input::InputManager::UpdateBindingWindows(uint32_t(wParam), true);
			//swprintf_s(msg, L"WM_KEYDOWN: 0x%x\n", wParam); //Debug
			//OutputDebugString(msg);
            return 0;
        }
		case WM_KEYUP:
		{
            // lParam bit 30 indicates the previous key flag, which is set to 1 for repeated key-down messages
            Input::InputManager::UpdateBindingWindows(uint32_t(wParam), false);
			return 0;
		}
		case WM_CHAR:
        {
            // We don't need this right now...
            // But it may come in handy later if there's some typings.
			return 0;
        }
*/
		case WM_SIZE:
		{
            // TODO: Seperate path for window init and window resize
            UINT width = LOWORD(lParam);
            UINT height = HIWORD(lParam);
            Input::InputManager::SetNewWindowSize(width, height);
            // TODO: 可能还是要使用Raw Input API来完美处理输入
            // SetCursorPos(back.x, back.y);

            ShowCursor(false);

            RECT winRect;
            GetWindowRect(hwnd, &winRect);
            ClipCursor(&winRect);
            SetCapture(hwnd);

            // TODO: 给Renderer发送重新创建SwapChain的信息
            return 0;
		}
		case WM_CLOSE:
		{
			Win32Window* pThis = (Win32Window*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
            ShowCursor(true);
            pThis->bContinue = false;
			return 0;
		}
		case WM_DESTROY:
		{
            ShowCursor(true);
            ClipCursor(NULL);
            ReleaseCapture();

			PostQuitMessage(0);
			return 0;
		}
    }
    // Default action for unhandled messages
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

}//namespace Window