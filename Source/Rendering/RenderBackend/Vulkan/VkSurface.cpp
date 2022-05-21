/*
    Description:
    The implementation of VkSurface module

    Created Date:
    5/13/2022 10:22:50 PM

	Notes:
	暂时把 window 创建的宽高写死在 Constructor 的 Initialization list 中
	暂时把RenderDoc插件放这里……（因为需要HWDN来激活）
*/

module;
#include <stdexcept>
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan\vulkan.h>
module VkSurface;

namespace RB
{

VkSurface::VkSurface()
	: windowWidth(800)
	, windowHeight(600)
	, window(windowWidth, windowHeight)
{}

VkSurface::~VkSurface()
{
	for (size_t i = 0; i < presentImageViews.size(); i++)
	{
		vkDestroyImageView(*devicePtr, presentImageViews[i], nullptr);
	}
	vkDestroySwapchainKHR(*devicePtr, vkSwapChain, nullptr);
	vkDestroySurfaceKHR(*instancePtr, vkSurface, nullptr);
}

void VkSurface::InitSurface(VkInstance* ins, VkPhysicalDevice* pDev, VkDevice* dev, uint32_t queueFamilyIndex)
{
	instancePtr = ins;
	pDevicePtr = pDev;
	devicePtr = dev;

	if (!InitWindowSurface(queueFamilyIndex))
	{
		throw std::runtime_error("Current selected queue doesn't support Presentation");
	}
	InitSwapChain();
	InitImageViews();
}

bool VkSurface::Update(float ms)
{
	return window.Update(ms);
}

bool VkSurface::InitWindowSurface(uint32_t queueFamilyIndex)
{
	VkWin32SurfaceCreateInfoKHR createInfo;
	createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	createInfo.pNext = nullptr;
	createInfo.flags = 0;
	createInfo.hinstance = window.GetInstance();
	createInfo.hwnd = window.GetHWDN();

	if (VK_SUCCESS != vkCreateWin32SurfaceKHR(*instancePtr, &createInfo, nullptr, &vkSurface))
	{
		throw std::runtime_error("Failed to create Win32 Surface");
	}

	// Check if the queue is able to present images
	VkBool32 supported = false;
	if (VK_SUCCESS != vkGetPhysicalDeviceSurfaceSupportKHR(*pDevicePtr, queueFamilyIndex, vkSurface, &supported))
	{
		throw std::runtime_error("Failed to get surface presentation support");
	}
	return supported;
}

void VkSurface::InitSwapChain()
{
	//TODO: 需要从外面传很多配置参数进来……然后还要做各种检查……啊……
	VkSurfaceCapabilitiesKHR surfaceCap;
	if (VK_SUCCESS != vkGetPhysicalDeviceSurfaceCapabilitiesKHR(*pDevicePtr, vkSurface, &surfaceCap))
	{
		throw std::runtime_error("Failed to get surface capabilities of physical device");
	}

	uint32_t supportedFormatCount = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR(*pDevicePtr, vkSurface, &supportedFormatCount, nullptr);
	std::vector<VkSurfaceFormatKHR> surfaceFormats(supportedFormatCount);
	vkGetPhysicalDeviceSurfaceFormatsKHR(*pDevicePtr, vkSurface, &supportedFormatCount, surfaceFormats.data());
	// CheckFormat(requiredFormat, availableFormat) 总之要选一个可用的format出来

	swapChainImageFormat = VK_FORMAT_R8G8B8A8_UNORM;
	swapChainExtent = surfaceCap.currentExtent;

	// Swap Chain算是相当重型了……如下好多参数都决定了整体的基调,需要多加重视
	VkSwapchainCreateInfoKHR createInfo;
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.pNext = nullptr;
	createInfo.flags = 0;
	createInfo.surface = vkSurface;
	createInfo.minImageCount = 3; //at least 3
	createInfo.imageFormat = swapChainImageFormat;
	createInfo.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR; // 现在有一大堆 Color Space...
	createInfo.imageExtent = swapChainExtent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	createInfo.queueFamilyIndexCount = 0;  // ignored if using exclusive mode
	createInfo.pQueueFamilyIndices = nullptr; // ignored if using exclusive mode
	createInfo.preTransform = surfaceCap.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
	createInfo.clipped = VK_TRUE; // Don't render into the invisible region
	createInfo.oldSwapchain = vkSwapChain;

	if (VK_SUCCESS != vkCreateSwapchainKHR(*devicePtr, &createInfo, nullptr, &vkSwapChain))
	{
		throw std::runtime_error("Failed to create Swap Chain");
	}

	uint32_t swapChainImageCount = 0;
	vkGetSwapchainImagesKHR(*devicePtr, vkSwapChain, &swapChainImageCount, nullptr);
	vkSwapChainImages.resize(swapChainImageCount);
	vkGetSwapchainImagesKHR(*devicePtr, vkSwapChain, &swapChainImageCount, vkSwapChainImages.data());
}

void VkSurface::InitImageViews()
{
	presentImageViews.resize(vkSwapChainImages.size());
	for (size_t i = 0; i < vkSwapChainImages.size(); i++)
	{
		VkImageViewCreateInfo createInfo;
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.pNext = nullptr;
		createInfo.flags = 0;
		createInfo.image = vkSwapChainImages[i];
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = swapChainImageFormat;
		createInfo.components = VkComponentMapping{};
		createInfo.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };

		if (VK_SUCCESS != vkCreateImageView(*devicePtr, &createInfo, nullptr, &presentImageViews[i]))
		{
			throw std::runtime_error("Failed to create present image view");
		}
	}
}

uint32_t VkSurface::GetAvailableImageIndex(uint64_t waitTimeNano, VkSemaphore semaphore, VkFence fence) const
{
	// TODO: 记得给这个参数,后续还要把Semaphore和Fence作为参数传进来
	// 或许不应该result image Index 不然switch case 里面不好 return
	uint32_t imageIndex = 0;
	VkResult result = vkAcquireNextImageKHR(*devicePtr, vkSwapChain, waitTimeNano, semaphore, fence, &imageIndex);
	switch (result)
	{
	case VK_SUCCESS:
	{
		return imageIndex;
	}
	case VK_NOT_READY:
	{
		// do something
	}break;
	default:
	{
		// do what?
	}
	};

	return imageIndex;
}

} //namespace RB