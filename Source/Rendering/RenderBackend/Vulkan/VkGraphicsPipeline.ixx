/*
    Description:
    The interface of VkPipelineGraphics module

    Created Date:
    5/9/2022 11:10:47 PM

    Notes:
*/

export module VkGraphicsPipeline;

import CommonAbstractionClass;
import Pipeline;
import VkSurface;
import VkRenderResource;
//import Model;
import std;
import <stdint.h>;
import <vulkan\vulkan.h>;

namespace RB
{

export class VkGraphicsPipeline : public Pipeline
{
	// Vulkan Graphics Pipeline Object
	VkPipeline graphicsPipeline;

	struct ShaderInfo
	{
		VkShaderModule mod;
		VkShaderStageFlagBits type;
		const char* entryName;
		ShaderInfo(VkShaderModule m, VkShaderStageFlagBits t, const char* n)
			: mod(m), type(t), entryName(n)
		{}
	};
	// Created Shaders
	std::vector<ShaderInfo> shaderModules;

	// RenderPass
	VkRenderPass renderPass;
	std::vector<VkFramebuffer> framebuffers;

public:
	VkGraphicsPipeline();
	VkGraphicsPipeline(VkPhysicalDevice*, VkDevice*, VkSurface*, const VkResourceLayout*, const ShaderArray&);
	~VkGraphicsPipeline();

    virtual PipelineType GetType() final override { return PipelineType::Graphics; }

	VkPipeline GetPipeline() { return graphicsPipeline; }

	VkSurface& GetSurfaceRef() const{ return *surface; }
	const VkFramebuffer& GetFrameBuffer(uint32_t i) const { return framebuffers[i]; }
	const VkRenderPass& GetRenderPass() const { return renderPass; }

	VkPipelineLayout GetPipelineLayout() { return resourceLayout->GetPipelineLayout(); }
	const VkDescriptorSet* GetDescriptorSet() { return resourceLayout->GetDescriptorSet(); }

	uint32_t GetPushConstantsSize() { return resourceLayout->GetPushConstantsSize(); }
	void* SetPushConstant(uint32_t i, VkShaderStageFlags& s, uint32_t& size)
		{ return resourceLayout->SetPushConstant(i, s, size); }

	// Model
	const VkBuffer* GetVertexBufferPtr() { return resourceLayout->GetVertexBufferPtr(); }
	const VkBuffer* GetIndexBufferPtr() { return resourceLayout->GetIndexBufferPtr(); }
	uint32_t GetVertexCount() { return resourceLayout->GetVertexCount(); }
	uint32_t GetIndexCount() { return resourceLayout->GetIndexCount(); }
private:
	// Temp References
	VkPhysicalDevice* pDevicePtr;
	VkDevice* devicePtr;
	VkSurface* surface;
	VkResourceLayout* resourceLayout;

	//Temp
	void CreateRenderPass();
	void CreateFramebuffer();

	void CreateShaderModule(const ShaderArray&);
	void CreateGraphicsPipeline();
};




}