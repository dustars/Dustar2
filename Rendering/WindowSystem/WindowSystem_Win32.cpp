/*
    Module Description:
    The implementation of Module partition uint WindowSystem Win32.

    Author:
    Zhao Han Ning

    Created Date:
    2022.4.17

    Last Updated:
    2022.4.18

    Notes:
*/

module;

#include <stdexcept>
#include <strsafe.h>

module WindowSystem:Win32;

import <Windows.h>;

namespace Window
{

Win32Window::Win32Window()
{
    constexpr wchar_t CLASS_NAME[]  = L"Window Class";

	// Register the window class.
	WNDCLASS wc = { };

	HINSTANCE instance = GetModuleHandle( NULL );

	wc.lpfnWndProc   = (WNDPROC)WindowProc;
	wc.hInstance     = instance;
	wc.lpszClassName = CLASS_NAME;

    if(!RegisterClass(&wc)) {
        throw std::runtime_error("Win32Window::Win32Window(): Failed to register class!");
        return;
    }

    // This is where the window is actually created and returned by OS as a handle
	window = CreateWindowEx(
        0,                              // Optional window styles.
        CLASS_NAME,                     // Window class
        L"VulkanWindow",                // Window text
        WS_OVERLAPPEDWINDOW|WS_POPUP|WS_VISIBLE|WS_SYSMENU|WS_MAXIMIZEBOX|WS_MINIMIZEBOX,            // Window style

        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

        NULL,       // Parent window    
        NULL,       // Menu
        instance,   // Instance handle
        NULL        // Additional application data
    );

    // The Following code is very useful to show error message when something goes horii
    //LPVOID lpMsgBuf;
    //LPVOID lpDisplayBuf;
    //DWORD dw = GetLastError(); 

    //FormatMessage(
    //    FORMAT_MESSAGE_ALLOCATE_BUFFER | 
    //    FORMAT_MESSAGE_FROM_SYSTEM |
    //    FORMAT_MESSAGE_IGNORE_INSERTS,
    //    NULL,
    //    dw,
    //    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
    //    (LPTSTR) &lpMsgBuf,
    //    0, NULL );

    //lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT, 
    //    (lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)"Error") + 40) * sizeof(TCHAR)); 
    //StringCchPrintf((LPTSTR)lpDisplayBuf, 
    //    LocalSize(lpDisplayBuf) / sizeof(TCHAR),
    //    TEXT("%s failed with error %d: %s"), 
    //    (LPCTSTR)"Error", dw, lpMsgBuf); 
    //MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK); 

    //LocalFree(lpMsgBuf);
    //LocalFree(lpDisplayBuf);
    //ExitProcess(dw); 

    if(!window) throw std::runtime_error("Cannot Create Window");
}

bool Win32Window::Update() const
{
    MSG msg = { };

    bool res;

    if ( res = GetMessage(&msg, NULL, 0, 0) )
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return res;
}

LRESULT Win32Window::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            // All painting occurs here, between BeginPaint and EndPaint.

            FillRect(hdc, &ps.rcPaint, (HBRUSH) (COLOR_WINDOW+1));

            EndPaint(hwnd, &ps);
        }
        return 0;
    }
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

}//namespace Window