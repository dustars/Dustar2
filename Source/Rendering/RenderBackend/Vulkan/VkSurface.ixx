/*
    Description:
    The interface of VkSurface module

    Created Date:
    5/13/2022 10:22:37 PM

    Notes:
*/
export module VkSurface;

import std;
import <vulkan\vulkan.h>;

namespace RB
{

export class VkSurface
{
	// 保存一些在整个渲染流程中统一的数据
	uint32_t windowWidth;
	uint32_t windowHeight;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;

	// Vulkan Objects Handle
	VkSurfaceKHR vkSurface;
	VkSwapchainKHR vkSwapChain;
	std::vector<VkImage> vkSwapChainImages; //存储在SwapChain中的N张Images
	std::vector<VkImageView> presentImageViews;

public:
	void InitSurface(VkInstance*, VkPhysicalDevice*, VkDevice*, uint32_t);
	void DestroySurface();

	// TODO: 临时为了ImageLayout Transition开放一下
	VkImage& GetImage(uint32_t i) { return vkSwapChainImages[i]; }; //从窗口系统获取一张可用的Image

	uint32_t GetAvailableImageIndex(uint64_t waitTimeNano, VkSemaphore sem, VkFence fence = VK_NULL_HANDLE) const; //从窗口系统获取一张可用的Image
	VkImageView* GetAvailableImageView(uint32_t i) { return &presentImageViews[i]; }; //从窗口系统获取一张可用的Image
	uint32_t GetNumImages() { return presentImageViews.size(); }
	const VkSwapchainKHR& GetSwapChain() { return vkSwapChain; }
	const VkFormat& GetFormat() { return swapChainImageFormat; }
	const VkExtent2D& GetExtent() const { return swapChainExtent; }

private:
	// Acquired after Vulkan initialization
	VkInstance* instancePtr;
	VkPhysicalDevice* pDevicePtr;
	VkDevice* devicePtr;

	bool InitWindowSurface(uint32_t queueFamilyIndex);
	void InitSwapChain();
	void InitImageViews();
};

}