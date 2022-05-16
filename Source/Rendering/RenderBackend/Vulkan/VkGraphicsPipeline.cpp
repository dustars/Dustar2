/*
    Description:
    The implementation of VkGraphicsPipeline module

    Created Date:
    5/15/2022 10:51:39 AM
*/

module;
#include <fstream>
#include <stdexcept>
module VkGraphicsPipeline;

import <vector>;
import <string>;
import <stdint.h>;
import <vulkan\vulkan.h>;

namespace RB
{

VkGraphicsPipeline::VkGraphicsPipeline() {}

VkGraphicsPipeline::VkGraphicsPipeline(VkPhysicalDevice* pDev, VkDevice* dev, VkSurface* sur, const VkResourceLayout* layout, const ShaderArray& shaders)
	: pDevicePtr(pDev)
	, devicePtr(dev)
	, surface(sur)
	, resourceLayout(const_cast<VkResourceLayout*>(layout))
{
	CreateRenderPass();
	CreateFramebuffer();
	// 这个要放外面可能
	CreateVertexBuffer();
	CreateShaderModule(shaders);
	// Create Shaders, Vertex & Fragment Shaders must exist. Others are optional.
	CreateGraphicsPipeline();
}

VkGraphicsPipeline::~VkGraphicsPipeline()
{
	vkDestroyRenderPass(*devicePtr, renderPass, nullptr);
	for (uint32_t i = 0; i < framebuffers.size(); i++)
	{
		vkDestroyFramebuffer(*devicePtr, framebuffers[i], nullptr);
	}
	vkDestroyBuffer(*devicePtr, vertexBuffer, nullptr);
	vkFreeMemory(*devicePtr, vertexMemory, nullptr);
	vkDestroyPipeline(*devicePtr, graphicsPipeline, nullptr);
	// 暂时由每个Pipeline负责删掉自己会用到的各种资源
	resourceLayout->DestroyResource(*devicePtr);
	delete resourceLayout; 
	for (uint32_t i = 0; i < shaderModules.size(); i++)
	{
		vkDestroyShaderModule(*devicePtr, shaderModules[i].mod, nullptr);
	}
}

void VkGraphicsPipeline::CreateShaderModule(const ShaderArray& shaders)
{
	for (uint32_t i = 0; i < shaders.size(); i++)
	{
		VkShaderStageFlagBits shaderType;
		switch (shaders[i].type)
		{
		case ShaderType::VS: {shaderType = VK_SHADER_STAGE_VERTEX_BIT; break;}
		case ShaderType::FS: {shaderType = VK_SHADER_STAGE_FRAGMENT_BIT; break;}
		case ShaderType::GS: {shaderType = VK_SHADER_STAGE_GEOMETRY_BIT; break;}
		case ShaderType::TCS: {shaderType = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT; break;}
		case ShaderType::TES: {shaderType = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT; break;}
		}
		shaderModules.emplace_back(VK_NULL_HANDLE, shaderType, shaders[i].entry.data());

		if (shaders[i].shaderFile.empty()) return;

		std::ifstream file(shaders[i].shaderFile, std::ios::ate | std::ios::binary);

		if (!file.is_open()) {
			throw std::runtime_error("failed to open file!");
		}

		size_t codeSize = (size_t)file.tellg();
		std::vector<char> code(codeSize);

		file.seekg(0);
		file.read(code.data(), codeSize);
		file.close();

		VkShaderModuleCreateInfo createInfo =
		{
			VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
			nullptr,
			0,
			codeSize,
			reinterpret_cast<const uint32_t*>(code.data())
		};

		if (VK_SUCCESS != vkCreateShaderModule(*devicePtr, &createInfo, nullptr, &shaderModules[i].mod))
		{
			throw std::runtime_error("Failed to create shader module");
		}
	}
}

void VkGraphicsPipeline::CreateRenderPass()
{
	VkAttachmentDescription attachmentDes;
	attachmentDes.flags = 0; // for memory aliasing
	attachmentDes.format = surface->GetFormat();
	attachmentDes.samples = VK_SAMPLE_COUNT_1_BIT;
	attachmentDes.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachmentDes.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachmentDes.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE; // Depth is with above two
	attachmentDes.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachmentDes.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	attachmentDes.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference attachmentRef;
	attachmentRef.attachment = 0;
	attachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpassDes;
	subpassDes.flags = 0;
	subpassDes.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpassDes.inputAttachmentCount = 0;
	subpassDes.pInputAttachments = nullptr;
	subpassDes.colorAttachmentCount = 1;
	subpassDes.pColorAttachments = &attachmentRef;
	subpassDes.pResolveAttachments = nullptr; // for multiSampling
	subpassDes.pDepthStencilAttachment = nullptr;
	subpassDes.preserveAttachmentCount = 0;
	subpassDes.pPreserveAttachments = nullptr;

	// Resources dependency between subpasses.
	//VkSubpassDependency subpassDen;
	//subpassDen.srcSubpass = VK_SUBPASS_EXTERNAL;
	//subpassDen.dstSubpass = 0;
	//subpassDen.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	//subpassDen.dstStageMask = 0;
	//subpassDen.srcAccessMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	//subpassDen.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	//subpassDen.dependencyFlags = ?;

	VkRenderPassCreateInfo createInfo;
	createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.flags = 0;
	createInfo.attachmentCount = 1;
	createInfo.pAttachments = &attachmentDes;
	createInfo.subpassCount = 1;
	createInfo.pSubpasses = &subpassDes;
	createInfo.dependencyCount = 0;
	createInfo.pDependencies = nullptr;

	if (VK_SUCCESS != vkCreateRenderPass(*devicePtr, &createInfo, nullptr, &renderPass))
	{
		throw std::runtime_error("Failed to create Render Pass");
	}
}

void VkGraphicsPipeline::CreateFramebuffer()
{
	framebuffers.resize(surface->GetNumImages());
	for (size_t i = 0; i < surface->GetNumImages(); i++)
	{
		VkFramebufferCreateInfo createInfo;
		createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		createInfo.pNext = nullptr;
		createInfo.flags = 0;
		createInfo.renderPass = renderPass;
		createInfo.attachmentCount = 1;
		// 千万注意，RenderPass里Attachment References就是会按照创建的顺序作为indices访问如下的array
		// 可以说是默认两边顺序要一样！千万不要弄错！！！
		createInfo.pAttachments = surface->GetAvailableImageView(i);
		createInfo.width = surface->GetExtent().width;
		createInfo.height = surface->GetExtent().height;
		createInfo.layers = 1;

		if (VK_SUCCESS != vkCreateFramebuffer(*devicePtr, &createInfo, nullptr, &framebuffers[i]))
		{
			throw std::runtime_error("Failed to create Framebuffer");
		}
	}
}

void VkGraphicsPipeline::CreateVertexBuffer()
{
	VkBufferCreateInfo createInfo;
	createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.flags = 0;
	createInfo.size = sizeof(Vertex) * 3;
	createInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
	createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	createInfo.queueFamilyIndexCount = 0; //ignored
	createInfo.pQueueFamilyIndices = nullptr;

	if (VK_SUCCESS != vkCreateBuffer(*devicePtr, &createInfo, nullptr, &vertexBuffer))
	{
		throw std::runtime_error("Failed to Vertex Buffer");
	}

	// Check Requirement
	VkMemoryRequirements memoryRequirements;
	vkGetBufferMemoryRequirements(*devicePtr, vertexBuffer, &memoryRequirements);

	// Find Suitable Memory Type
	uint32_t memoryIndex = FindMemoryType(
		memoryRequirements,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
	);

	// Allocate Memory
	VkMemoryAllocateInfo allocInfo;
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.pNext = nullptr;
	allocInfo.allocationSize = 1024 * 1024 * sizeof(float);
	allocInfo.memoryTypeIndex = memoryIndex;

	if (VK_SUCCESS != vkAllocateMemory(*devicePtr, &allocInfo, nullptr, &vertexMemory))
	{
		throw std::runtime_error("Failed to create device memory");
	}

	{ // Mapping
		void* mappedData;
		if (VK_SUCCESS != vkMapMemory(*devicePtr, vertexMemory, 0, VK_WHOLE_SIZE, 0, &mappedData))
		{
			throw std::runtime_error("Failed to Map Memory");
		}

		memcpy(mappedData, vertexData.data(), sizeof(Vertex) * 3);

		// Flush the memory so the device can actually use the image data.
		VkMappedMemoryRange range;
		range.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
		range.pNext = nullptr;
		range.memory = vertexMemory;
		range.offset = 0;
		range.size = VK_WHOLE_SIZE;
		vkFlushMappedMemoryRanges(*devicePtr, 1, &range);

		vkUnmapMemory(*devicePtr, vertexMemory);
	}

	// Bind memory object and image object
	if (VK_SUCCESS != vkBindBufferMemory(*devicePtr, vertexBuffer, vertexMemory, 0))
	{
		throw std::runtime_error("Failed to bind buffer and memory");
	}
}

void VkGraphicsPipeline::CreateGraphicsPipeline()
{
	std::vector<VkPipelineShaderStageCreateInfo> shaderCreateInfo{};
	for (uint32_t i = 0; i < shaderModules.size(); i++)
	{
		shaderCreateInfo.emplace_back(
			VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			nullptr,
			0,
			shaderModules[i].type,
			shaderModules[i].mod,
			shaderModules[i].entryName,
			nullptr
		);
	}

	std::vector<VkVertexInputBindingDescription> bindings;
	bindings.emplace_back(
		0, //Binding Index
		sizeof(Vertex), //Stride 就是Vertex的数据结构大小
		VK_VERTEX_INPUT_RATE_VERTEX //Index or Instancing
	);

	std::vector<VkVertexInputAttributeDescription> attributes;
	attributes.emplace_back( // Vertex Position
		0, //Location
		0, //Binding Index
		VK_FORMAT_R32G32B32A32_SFLOAT,
		0
	);
	attributes.emplace_back( // Vertex UV
		1, //Location
		0, //Binding Index
		VK_FORMAT_R32G32_SFLOAT,
		sizeof(float) * 4
	);

	VkPipelineVertexInputStateCreateInfo vertexCreateInfo;
	vertexCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexCreateInfo.pNext = nullptr;
	vertexCreateInfo.flags = 0;
	vertexCreateInfo.vertexBindingDescriptionCount = bindings.size();
	vertexCreateInfo.pVertexBindingDescriptions = bindings.data();
	vertexCreateInfo.vertexAttributeDescriptionCount = attributes.size();
	vertexCreateInfo.pVertexAttributeDescriptions = attributes.data();

	VkPipelineInputAssemblyStateCreateInfo InputCreateInfo;
	InputCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	InputCreateInfo.pNext = nullptr;
	InputCreateInfo.flags = 0;
	InputCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	InputCreateInfo.primitiveRestartEnable = VK_FALSE;

	VkViewport viewport = { 0.f, 0.f, surface->GetExtent().width, surface->GetExtent().height, 0.f, 1.f };
	VkRect2D scissor = { {0,0} , surface->GetExtent() };
	VkPipelineViewportStateCreateInfo viewportCreateInfo;
	viewportCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportCreateInfo.pNext = nullptr;
	viewportCreateInfo.flags = 0;
	viewportCreateInfo.viewportCount = 1;
	viewportCreateInfo.pViewports = &viewport;
	viewportCreateInfo.scissorCount = 1;
	viewportCreateInfo.pScissors = &scissor;

	VkPipelineRasterizationStateCreateInfo rasterizationCreateInfo;
	rasterizationCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizationCreateInfo.pNext = nullptr;
	rasterizationCreateInfo.flags = 0;
	rasterizationCreateInfo.depthClampEnable = VK_FALSE;
	rasterizationCreateInfo.rasterizerDiscardEnable = VK_FALSE; //为啥这是个总控开关？？
	rasterizationCreateInfo.polygonMode = VK_POLYGON_MODE_FILL; //WireFrame Mode和点云 Mode启动！
	rasterizationCreateInfo.cullMode = VK_CULL_MODE_NONE; // 暂时不Cull 免得出错
	rasterizationCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterizationCreateInfo.depthBiasEnable = VK_FALSE;
	rasterizationCreateInfo.depthBiasConstantFactor = 0; // Optional
	rasterizationCreateInfo.depthBiasClamp = 0; // Optional
	rasterizationCreateInfo.depthBiasSlopeFactor = 0; // Optional
	rasterizationCreateInfo.lineWidth = 1.0f;

	VkPipelineMultisampleStateCreateInfo multisamplingCreateInfo{}; //我人傻了，为什么这个地方必须要加{}? 不然Pipeline创建老是失败？？
	multisamplingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisamplingCreateInfo.sampleShadingEnable = VK_FALSE;
	multisamplingCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisamplingCreateInfo.minSampleShading = 1.0f; // Optional
	multisamplingCreateInfo.pSampleMask = nullptr; // Optional
	multisamplingCreateInfo.alphaToCoverageEnable = VK_FALSE; // Optional
	multisamplingCreateInfo.alphaToOneEnable = VK_FALSE; // Optional

	VkPipelineColorBlendAttachmentState attachmentState;
	attachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	attachmentState.blendEnable = VK_FALSE;
	attachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
	attachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
	attachmentState.colorBlendOp = VK_BLEND_OP_ADD; // Optional
	attachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
	attachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
	attachmentState.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

	VkPipelineColorBlendStateCreateInfo blendCreateInfo{}; //我人傻了，为什么这个地方必须要加{}? 不然Pipeline创建老是失败？？
	blendCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	blendCreateInfo.logicOpEnable = VK_FALSE;
	blendCreateInfo.logicOp = VK_LOGIC_OP_COPY; // Optional
	blendCreateInfo.attachmentCount = 1;
	blendCreateInfo.pAttachments = &attachmentState;
	blendCreateInfo.blendConstants[0] = 0.0f; // Optional
	blendCreateInfo.blendConstants[1] = 0.0f; // Optional
	blendCreateInfo.blendConstants[2] = 0.0f; // Optional
	blendCreateInfo.blendConstants[3] = 0.0f; // Optional

	VkGraphicsPipelineCreateInfo pipelineCreateInfo;
	pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineCreateInfo.pNext = nullptr;
	pipelineCreateInfo.flags = VK_PIPELINE_CREATE_DISABLE_OPTIMIZATION_BIT;
	pipelineCreateInfo.stageCount = 2;
	pipelineCreateInfo.pStages = shaderCreateInfo.data();
	pipelineCreateInfo.pVertexInputState = &vertexCreateInfo;
	pipelineCreateInfo.pInputAssemblyState = &InputCreateInfo;
	pipelineCreateInfo.pTessellationState = nullptr;
	pipelineCreateInfo.pViewportState = &viewportCreateInfo;
	pipelineCreateInfo.pRasterizationState = &rasterizationCreateInfo;
	pipelineCreateInfo.pMultisampleState = &multisamplingCreateInfo;
	pipelineCreateInfo.pDepthStencilState = nullptr;
	pipelineCreateInfo.pColorBlendState = &blendCreateInfo;
	pipelineCreateInfo.pDynamicState = nullptr;
	pipelineCreateInfo.layout = resourceLayout->BuildPipelineLayout(*devicePtr);
	pipelineCreateInfo.renderPass = renderPass;
	pipelineCreateInfo.subpass = 0;
	pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
	pipelineCreateInfo.basePipelineIndex = -1;

	if (VK_SUCCESS != vkCreateGraphicsPipelines(*devicePtr, VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &graphicsPipeline))
	{
		throw std::runtime_error("Failed to create Graphics Pipeline");
	}
}

uint32_t VkGraphicsPipeline::FindMemoryType(const VkMemoryRequirements& memoryRequirements, VkMemoryPropertyFlags requiredFlags, VkMemoryPropertyFlags preferredFlags)
{
	VkPhysicalDeviceMemoryProperties memoryProperties;
	vkGetPhysicalDeviceMemoryProperties(*pDevicePtr, &memoryProperties);

	uint32_t selectedType = ~0u;
	uint32_t memoryType;

	for (memoryType = 0; memoryType < 32; memoryType++) {
		if (memoryRequirements.memoryTypeBits & (1 << memoryType))
		{
			const VkMemoryType& type = memoryProperties.memoryTypes[memoryType];

			if ((type.propertyFlags & preferredFlags) == preferredFlags)
			{
				selectedType = memoryType;
				break;
			}
		}
	}

	if (selectedType != ~0u)
	{
		for (memoryType = 0; memoryType < 32; memoryType++)
		{
			if (memoryRequirements.memoryTypeBits & (1 << memoryType))
			{
				const VkMemoryType& type = memoryProperties.memoryTypes[memoryType];

				if ((type.propertyFlags & requiredFlags) == requiredFlags)
				{
					selectedType = memoryType;
					break;
				}
			}
		}
	}

	return selectedType;
}

} //namespace RB