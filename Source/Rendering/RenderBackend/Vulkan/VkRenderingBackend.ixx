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

	// Temp
	typedef struct Vertex
	{
		float x, y, z, w;
		float u, v;
	};
	std::vector<Vertex> vertexData =
	{
		{0.f, 0.5f, 0.f, 1.f, 0.5f, 1.f},
		{-0.5f, -0.5f, 0.f, 1.f, 0.f, 0.f},
		{0.5f, -0.5f, 0.f, 1.f, 1.f, 0.f}
	};

	VkBuffer vertexBuffer;
	VkDeviceMemory vertexMemory;

	// Graphics Pipeline
	VkShaderModule vertShader;
	VkShaderModule fragShader;
	VkPipeline graphicsPipeline;

	// RenderPass
	VkRenderPass renderPass;
	std::vector<VkFramebuffer> framebuffers;

	// Layout
	VkPipelineLayout graphicsPipelineLayout;
	// Temp
    
public:
    VkRBInterface();
	~VkRBInterface();

	virtual bool Update() final override;
	virtual bool Render() final override;

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

	//Temp
	void CreateShaderModule(const std::string& shaderFile, VkShaderModule& mod);
	void CreateVertexBuffer();
	void CreateRenderPass();
	void CreateFramebuffer();
	void CreateGraphicsPipeline();
	uint32_t FindMemoryType(const VkMemoryRequirements& memoryRequirements, VkMemoryPropertyFlags requiredFlags, VkMemoryPropertyFlags preferredFlags);
};

} // namespace RB