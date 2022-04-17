export module WindowSystem:SurfaceBase;

import <vulkan\vulkan.h>;

namespace Window
{
	export class SurfaceBase
	{
	public:

		virtual void CreateSurface() = 0;

		VkSurfaceKHR surface;
	};
}// namespace Window