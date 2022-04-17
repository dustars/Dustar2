export module WindowSystem;

export import :Win32;
// TODO: Inplement the windows system on Xlib
export import :Xlib;

namespace Window
{

export class Window
{


public:
	Window();

	//export virtual void GetSurface() { return surface; }

private:
	//VkSurfaceKHR surface;



};	// class Window

}	// namespace Window