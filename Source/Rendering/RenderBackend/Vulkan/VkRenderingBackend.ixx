/*
    Description:
    The interface of VkRenderingBackend module

    Created Date:
    5/10/2022 8:49:58 PM
*/
export module VkRenderingBackend;

import RenderingBackend;
import VkSurface;
import VkCmdBuffer;
import Pipeline;
import VkGraphicsPipeline;
import VkComputePipeline;
import <vector>;
import <string>;
import <stdint.h>;
import <vulkan\vulkan.h>;

namespace RB
{

export class VkRBInterface : public RBInterface
{
	// Vulkan Initialization objects
	VkInstance vkInstance;
	VkPhysicalDevice vkPhysicalDevice;
	VkDevice vkDevice;
	uint32_t currentQueueFamilyIndex; //目前一个 queue 打天下……还没 Multithreading
	uint32_t queueNumbers = 1;
	std::vector<VkQueue> vkQueues;

	// Vulkan side's window buffer
	VkSurface* surface;
	VkCmdBuffer* cmd;

	// Temp Synchronization
	VkSemaphore imageAvailableSemaphore;
	VkSemaphore renderFinishedSemaphore;
	VkFence inFlightFence;

	std::vector<VkGraphicsPipeline> testGraphicsPipeline;
	std::vector<VkGraphicsPipeline> testComputePipelines;

public:
    VkRBInterface();
	~VkRBInterface();

	virtual bool Update() final override;
	virtual bool Render() final override;

	virtual Pipeline& CreateGraphicsPipeline() final override;
	virtual Pipeline& CreateComputePipeline() final override;

private:
	// Initialization
	void InitVulkanInstance();
	void InitVulkanPhysicalDevices();
	void InitVulkanLogicalDevice();
	void InitVulkanQueues();
	void InitVulkanSynchronizations();

	void EnableInstanceLayers(std::vector<const char*>&);
	void EnableInstanceExtensions(std::vector<const char*>&);
	void EnableDeviceLayers(std::vector<const char*>&);
	void EnableDeviceExtensions(std::vector<const char*>&);

	bool FindSubitableQueueFamily(); // For single threading only

	// Run time operations
	void WindowPresentation(uint32_t imageIndex);
};

} // namespace RB