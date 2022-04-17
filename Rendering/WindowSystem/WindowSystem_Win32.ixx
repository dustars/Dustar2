export module WindowSystem:Win32;

import :SurfaceBase;
import <Windows.h>;

namespace Window
{
	class Win32Surface : public SurfaceBase
	{
	public:
		void CreateSurface() override;
	};
}	//namespace Window::Win32