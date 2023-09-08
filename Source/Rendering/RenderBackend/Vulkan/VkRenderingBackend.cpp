/*
    Description:
    The implementation of VkRenderingBackend module

    Created Date:
    5/10/2022 8:50:08 PM
*/

module;
#define WINDOW_APP
#define VK_USE_PLATFORM_WIN32_KHR
module VkRenderingBackend;

import Input;
import RenderDocPlugin;
import <vulkan\vulkan.h>;

namespace RB
{

VkRBInterface::VkRBInterface()
{
	InitVulkanInstance();
	InitVulkanPhysicalDevices();
	InitVulkanLogicalDevice();
	InitVulkanQueues();
	InitVulkanSynchronizations();

	surface.InitSurface(&vkInstance, &vkPhysicalDevice, &vkDevice, currentQueueFamilyIndex);
	cmd.InitCommandBuffer(&vkDevice, currentQueueFamilyIndex);
}

VkRBInterface::~VkRBInterface()
{
	// 必须提前删掉pipelines 各种依赖关系你懂的
	testGraphicsPipeline.clear();
	// 必须在删除device和instance之前delete cmd 和 surface 各种依赖关系你懂的
	cmd.DestroyCommandBuffer();
	surface.DestroySurface();
	vkDestroySemaphore(vkDevice, imageAvailableSemaphore, nullptr);
	vkDestroySemaphore(vkDevice, renderFinishedSemaphore, nullptr);
	vkDestroyFence(vkDevice, inFlightFence, nullptr);
	vkDestroyDevice(vkDevice, nullptr);
	vkDestroyInstance(vkInstance, nullptr);
}

bool VkRBInterface::Update(float ms)
{
	return true;
}

bool VkRBInterface::Render()
{
	//Draw Graphics Pipeline
	for (uint32_t i = 0; i < testGraphicsPipeline.size(); i++)
	{
		vkWaitForFences(vkDevice, 1, &inFlightFence, VK_TRUE, UINT64_MAX);
		vkResetFences(vkDevice, 1, &inFlightFence);

		uint32_t imageIndex = surface.GetAvailableImageIndex(UINT64_MAX, imageAvailableSemaphore);

		cmd.BeginCommandBuffer();
		cmd.BeginRenderPass(testGraphicsPipeline[i], imageIndex);

		renderingOps[i](&cmd);

		cmd.EndRenderPass();
		cmd.EndCommandBuffer();
		cmd.SubmitCommandBuffer(vkQueues[0], imageAvailableSemaphore, renderFinishedSemaphore);

		WindowPresentation(imageIndex);
		// 相当重要……
		vkQueueWaitIdle(vkQueues[0]);
	}

	vkDeviceWaitIdle(vkDevice);
	return true;
}

Pipeline& VkRBInterface::CreateGraphicsPipeline(const ResourceLayout* layout, const ShaderArray& shaders)
{
	return dynamic_cast<Pipeline&>(testGraphicsPipeline.emplace_back(
		&vkPhysicalDevice,
		&vkDevice,
		&surface,
		dynamic_cast<const VkResourceLayout*>(layout),
		shaders
	));
}

Pipeline& VkRBInterface::CreateComputePipeline(const ResourceLayout* layout, const ShaderFile& computeShader)
{
	return dynamic_cast<Pipeline&>(testComputePipelines.emplace_back(
	
	));
}

void VkRBInterface::InitResources(const std::vector<ResourceLayout*>& layouts)
{
	VkResourceLayout::CreateDescriptorPool(vkDevice);
	VkResourceLayout::CreateResources(vkDevice, vkPhysicalDevice);
	for (auto& layout : layouts)
	{
		dynamic_cast<VkResourceLayout*>(layout)->AllocateDescriptorSet();
		dynamic_cast<VkResourceLayout*>(layout)->BindResourcesAndDescriptors();
	}
}

ResourceLayout* VkRBInterface::CreateResourceLayout()
{
	// 后端创建, Manager删除
	return new VkResourceLayout(vkDevice, vkPhysicalDevice);
}

void VkRBInterface::InitVulkanInstance()
{
	// 既然可以在创建Instance之前先获取Instance级别的Layer和Extensions,
	// 那是否代表着这两种东西是跟硬件无关的？取决于Vulkan在平台上的实现？
	std::vector<const char*> enabledInstanceLayers;
	EnableInstanceLayers(enabledInstanceLayers);

	std::vector<const char*> enabledInstanceExtensions;
	EnableInstanceExtensions(enabledInstanceExtensions);

	VkApplicationInfo appInfo;
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pNext = nullptr;
	appInfo.pApplicationName = "VulkanLearning";
	appInfo.applicationVersion = 1;
	appInfo.pEngineName = "NoEngine";
	appInfo.engineVersion = 0;
	// 操 这个apiVersion还挺复杂的 建议看文档……
	appInfo.apiVersion = 0;

	VkInstanceCreateInfo createInfo;
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.flags = 0;
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledLayerCount = static_cast<uint32_t>(enabledInstanceLayers.size());
	createInfo.ppEnabledLayerNames = enabledInstanceLayers.data();
	createInfo.enabledExtensionCount = static_cast<uint32_t>(enabledInstanceExtensions.size());
	createInfo.ppEnabledExtensionNames = enabledInstanceExtensions.data();

	auto info = vkCreateInstance(&createInfo, nullptr, &VkRBInterface::vkInstance);

	if (VK_SUCCESS != info)
	{
		throw std::runtime_error("failed to Create Vulkan Instance");
	}
}

void VkRBInterface::InitVulkanPhysicalDevices()
{
	uint32_t numPhysicalDevices = 0;

	if (VK_SUCCESS != vkEnumeratePhysicalDevices(vkInstance, &numPhysicalDevices, nullptr))
	{
		throw std::runtime_error("Failed to enumerate physical devices");
	}

	std::vector<VkPhysicalDevice> devices;
	devices.resize(numPhysicalDevices);
	vkEnumeratePhysicalDevices(vkInstance, &numPhysicalDevices, devices.data());
	vkPhysicalDevice = devices[0];

	// TODO: 
	// 1. Make a function to choose the best device if there's any.
	// 2. Find a way to save the physical devices info below,
	//    we'll need a way to determine some run-time behaviors
	//    based on these info.

	// Get physical device properties.
	VkPhysicalDeviceProperties properties;
	vkGetPhysicalDeviceProperties(vkPhysicalDevice, &properties);

	// Get physical device features.
	VkPhysicalDeviceFeatures features;
	vkGetPhysicalDeviceFeatures(vkPhysicalDevice, &features);

	// Get physical device memory properties.
	VkPhysicalDeviceMemoryProperties memoryProperties;
	vkGetPhysicalDeviceMemoryProperties(vkPhysicalDevice, &memoryProperties);
}

void VkRBInterface::InitVulkanLogicalDevice()
{
	// 太多Hardcoded了……未来慢慢丰富吧
	// Queue Family Index默认是0
	// Physical Device默认是0（目前我电脑也只有一个……也就是6900xt）
	std::vector<const char*> enabledDeviceLayers;
	EnableDeviceLayers(enabledDeviceLayers);

	std::vector<const char*> enabledDeviceExtensions;
	EnableDeviceExtensions(enabledDeviceExtensions);

	if (!FindSubitableQueueFamily())
	{
		throw std::runtime_error("Cannot find suitable queue!");
	}

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfo;
	float priorities[1] = { 0 };
	queueCreateInfo.emplace_back(
		VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
		nullptr,
		0,
		currentQueueFamilyIndex,
		1,
		priorities
	);

	VkDeviceCreateInfo createInfo;
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.flags = 0;
	// TODO: Queue的动态选择和创建
	createInfo.pQueueCreateInfos = queueCreateInfo.data();
	createInfo.queueCreateInfoCount = queueCreateInfo.size();
	// TODO: layer和extension的动态加载
	createInfo.enabledLayerCount = static_cast<uint32_t>(enabledDeviceLayers.size());
	createInfo.ppEnabledLayerNames = enabledDeviceLayers.data();
	createInfo.enabledExtensionCount = static_cast<uint32_t>(enabledDeviceExtensions.size());
	createInfo.ppEnabledExtensionNames = enabledDeviceExtensions.data();
	// TODO: 可以把Physical Device创建的feature直接拿来用,！但会增加性能开销！
	// 一个更加自然的实现就是用到什么就enable什么feature,但这个就比较复杂了……
	createInfo.pEnabledFeatures = nullptr;

	if (VK_SUCCESS != vkCreateDevice(vkPhysicalDevice, &createInfo, nullptr, &vkDevice))
	{
		throw std::runtime_error("Failed to create logical device");
	}
}

void VkRBInterface::InitVulkanQueues()
{
	vkQueues.resize(1);
	vkGetDeviceQueue(vkDevice, currentQueueFamilyIndex, 0, &vkQueues[0]);
	if (vkQueues[0] == VK_NULL_HANDLE)
	{
		throw std::runtime_error("Failed to Get Queue");
	}
}

void VkRBInterface::InitVulkanSynchronizations()
{
	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
	if (vkCreateSemaphore(vkDevice, &semaphoreInfo, nullptr, &imageAvailableSemaphore) != VK_SUCCESS ||
		vkCreateSemaphore(vkDevice, &semaphoreInfo, nullptr, &renderFinishedSemaphore) != VK_SUCCESS ||
		vkCreateFence(vkDevice, &fenceInfo, nullptr, &inFlightFence) != VK_SUCCESS) {
		throw std::runtime_error("failed to create semaphores!");
	}
}

void VkRBInterface::EnableInstanceLayers(std::vector<const char*>& enabledInstanceLayers)
{
	//TODO: 后期会需要一个机制在创建APP前确定需要哪些layers & extensions
#ifndef NDEBUG
	enabledInstanceLayers.push_back("VK_LAYER_KHRONOS_validation");
#endif

	uint32_t layerCount;
	std::vector<VkLayerProperties> layers;

	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
	if (layerCount != 0)
	{
		layers.resize(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, layers.data());
	}

	// TODO：获取到该Platform支持的Layers后,要检查是否覆盖了APP所需
	// CheckIfRequiredLayersAvailable()
}

void VkRBInterface::EnableInstanceExtensions(std::vector<const char*>& enabledInstanceExtension)
{
	// TODO: Full screen / Borderless window
#ifdef WINDOW_APP
	enabledInstanceExtension.push_back("VK_KHR_surface");

#ifdef VK_USE_PLATFORM_WIN32_KHR
	enabledInstanceExtension.push_back("VK_KHR_win32_surface");
#endif

#endif

	uint32_t extensionCount;
	std::vector<VkExtensionProperties> extensions;

	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
	if (extensionCount != 0)
	{
		extensions.resize(extensionCount);
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
	}

	// TODO：获取到该Platform支持的Extensions后,要检查是否覆盖了APP所需
	// CheckIfRequiredExtentsionAvailable()
}

void VkRBInterface::EnableDeviceLayers(std::vector<const char*>& enabledDeviceLayers)
{
	uint32_t layerCount;
	std::vector<VkLayerProperties> layers;

	vkEnumerateDeviceLayerProperties(vkPhysicalDevice, &layerCount, nullptr);
	if (layerCount != 0)
	{
		layers.resize(layerCount);
		vkEnumerateDeviceLayerProperties(vkPhysicalDevice, &layerCount, layers.data());
	}
}

void VkRBInterface::EnableDeviceExtensions(std::vector<const char*>& enabledDeviceExtensions)
{
#ifdef WINDOW_APP
	enabledDeviceExtensions.push_back("VK_KHR_swapchain");
#endif

	uint32_t extensionCount;
	std::vector<VkExtensionProperties> extensions;

	vkEnumerateDeviceExtensionProperties(vkPhysicalDevice, nullptr, &extensionCount, nullptr);
	if (extensionCount != 0)
	{
		extensions.resize(extensionCount);
		vkEnumerateDeviceExtensionProperties(vkPhysicalDevice, nullptr, &extensionCount, extensions.data());
	}
}

bool VkRBInterface::FindSubitableQueueFamily()
{
	// Get queue family properties of the physical device
	uint32_t queueFamilyCount;
	vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, &queueFamilyCount, queueFamilyProperties.data());

	currentQueueFamilyIndex = 0;
	for (auto& entry : queueFamilyProperties)
	{
		return entry.queueFlags & (VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT);
		currentQueueFamilyIndex++;
	}
	return false;
}

void VkRBInterface::WindowPresentation(uint32_t imageIndex)
{
	VkResult result;
	VkPresentInfoKHR presentInfo;
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.pNext = nullptr;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &renderFinishedSemaphore;
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &surface.GetSwapChain();
	presentInfo.pImageIndices = &imageIndex;
	presentInfo.pResults = &result;

	if (VK_SUCCESS != vkQueuePresentKHR(vkQueues[0], &presentInfo))
	{
		throw std::runtime_error("Failed to present the Image");
	}

	if (VK_SUCCESS != result)
	{
		throw std::runtime_error("Presentation has gone wrong!");
	}
}

} //namespace RB