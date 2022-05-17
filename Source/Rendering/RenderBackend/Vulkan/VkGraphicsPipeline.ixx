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
import <vector>;
import <string>;
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

	// Tempppppppppppppppppppppppppppppppp
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

	// Layout
	VkPipelineLayout graphicsPipelineLayout;

	// RenderPass
	VkRenderPass renderPass;
	std::vector<VkFramebuffer> framebuffers;

public:
	VkGraphicsPipeline();
	VkGraphicsPipeline(VkPhysicalDevice*, VkDevice*, VkSurface*, const VkResourceLayout*, const ShaderArray&);
	~VkGraphicsPipeline();

    virtual PipelineType GetType() final override { return PipelineType::Graphics; }

	VkPipeline GetPipeline() { return graphicsPipeline; }
	VkBuffer* GetVertexBufferPtr() { return &vertexBuffer; }
	uint32_t GetVertexDataSize() { return vertexData.size(); }
	const VkSurface& GetSurfaceRef() const{ return *surface; }
	const VkFramebuffer& GetFrameBuffer(uint32_t i) const { return framebuffers[i]; }
	const VkRenderPass& GetRenderPass() const { return renderPass; }

	VkPipelineLayout GetPipelineLayout() { return resourceLayout->GetPipelineLayout(); }
	const VkDescriptorSet* GetDescriptorSet() { return resourceLayout->GetDescriptorSet(); }
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
	void CreateVertexBuffer();
	void CreateGraphicsPipeline();
};




}