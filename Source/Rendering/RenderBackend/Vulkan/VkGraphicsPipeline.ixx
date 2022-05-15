/*
    Description:
    The interface of VkPipelineGraphics module

    Created Date:
    5/9/2022 11:10:47 PM

    Notes:
*/

export module VkGraphicsPipeline;

import Pipeline;
import VkSurface;
import <vector>;
import <string>;
import <stdint.h>;
import <vulkan\vulkan.h>;

namespace RB
{

export class VkGraphicsPipeline : public Pipeline
{
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
	// Layout
	VkPipelineLayout graphicsPipelineLayout;

	// RenderPass
	VkRenderPass renderPass;
	std::vector<VkFramebuffer> framebuffers;

public:
	VkGraphicsPipeline();
	VkGraphicsPipeline(VkPhysicalDevice*, VkDevice*, VkSurface*);
	~VkGraphicsPipeline();

    virtual PipelineType GetType() final override { return PipelineType::Graphics; }

	VkPipeline GetPipeline() { return graphicsPipeline; }
	VkBuffer* GetVertexBufferPtr() { return &vertexBuffer; }
	uint32_t GetVertexDataSize() { return vertexData.size(); }
	const VkSurface& GetSurfaceRef() const{ return *surface; }
	const VkFramebuffer& GetFrameBuffer(uint32_t i) const { return framebuffers[i]; }
	const VkRenderPass& GetRenderPass() const { return renderPass; }
private:
	// Temp References
	VkPhysicalDevice* pDevicePtr;
	VkDevice* devicePtr;
	VkSurface* surface;

	//Temp
	void CreateRenderPass();
	void CreateFramebuffer();

	void CreateShaderModule(const std::string& shaderFile, VkShaderModule& mod);
	void CreateVertexBuffer();
	void CreateGraphicsPipeline(
		// Shaders
		const std::string& vert,
		const std::string& frag,
		const std::string& geom = "",
		const std::string& ctrl = "",
		const std::string& eval = ""
	);
	uint32_t FindMemoryType(const VkMemoryRequirements& memoryRequirements, VkMemoryPropertyFlags requiredFlags, VkMemoryPropertyFlags preferredFlags);
};




}