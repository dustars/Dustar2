/*
    Description:
    The implementation of VkRenderResource module

    Created Date:
    5/16/2022 10:47:46 PM
*/

module;
#define STB_IMAGE_IMPLEMENTATION
#include "Utilities/stb/stb_image.h"
module VkRenderResource;

import <vulkan/vulkan.h>;


namespace RB
{

std::unordered_map<VkDescriptorType, uint32_t> VkResourceLayout::poolMembers;
VkDescriptorPool VkResourceLayout::descriptorPool;
std::vector<VkResourceLayout::ResourceInfo> VkResourceLayout::resourceInfo;
VkMemoryRequirements VkResourceLayout::bufferMemoryRequirements;
VkDeviceMemory VkResourceLayout::bufferMemory;
VkMemoryRequirements VkResourceLayout::imageSRVMemoryRequirements;
VkDeviceMemory VkResourceLayout::ImageMemory;
std::vector<VkResourceLayout::resEntry> VkResourceLayout::buffers;
std::vector<VkResourceLayout::resEntry> VkResourceLayout::Images;

VkResourceLayout::~VkResourceLayout()
{
	// Static variable只能调用一次，咋办……
	vkDestroyPipelineLayout(vkDevice, pipelineLayout, nullptr);
	vkDestroyDescriptorPool(vkDevice, descriptorPool, nullptr);
	vkDestroyDescriptorSetLayout(vkDevice, setLayout, nullptr);

	vkDestroyBuffer(vkDevice, vertexBuffer, nullptr);
	vkDestroyBuffer(vkDevice, indexBuffer, nullptr);
	vkFreeMemory(vkDevice, vertexMemory, nullptr);
	vkFreeMemory(vkDevice, indexMemory, nullptr);

	// TODO: 应该更早删掉...管他的,临时放下
	vkDestroyBuffer(vkDevice, stageBuffer, nullptr);
	vkFreeMemory(vkDevice, stageMemory, nullptr);
	vkDestroyBuffer(vkDevice, stageBufferForImage, nullptr);
	vkFreeMemory(vkDevice, stageMemoryForImage, nullptr);
	vkDestroySampler(vkDevice, testImageSampler, nullptr);
	vkDestroyImageView(vkDevice, testImageView, nullptr);

	for (auto& entry : buffers) vkDestroyBuffer(vkDevice, std::get<VkBuffer>(entry.res), nullptr);
	for (auto& entry : Images) vkDestroyImage(vkDevice, std::get<VkImage>(entry.res), nullptr);

	vkFreeMemory(vkDevice, bufferMemory, nullptr);
	vkFreeMemory(vkDevice, ImageMemory, nullptr);
}

template<typename T>
void VkResourceLayout::CreateVertexBuffer(const std::vector<T>& vertexData)
{
	vertexCount = vertexData.size();
	vertexByteSize = sizeof(T) * vertexCount;

if constexpr (useBufferAddressVertex)
{
	vertexBuffer = CreateBuffer("", vertexByteSize, BufferUsage::BUFFERADDR, nullptr);
}
else
{
	vertexBuffer = CreateBuffer("", vertexByteSize, BufferUsage::VERTEX, nullptr);
}

	{
		VkMemoryRequirements vertexMemoryRequirements;
		vkGetBufferMemoryRequirements(vkDevice, vertexBuffer, &vertexMemoryRequirements);
		// Find Suitable Memory Type
		uint32_t memoryIndex = FindMemoryType(
			vkPhysicalDevice,
			vertexMemoryRequirements,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);

		VkMemoryAllocateFlagsInfo extraInfo;
		extraInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO;
		extraInfo.pNext = nullptr;
		extraInfo.flags = VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT;

		// Allocate Memory
		VkMemoryAllocateInfo allocInfo;
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.pNext = nullptr;
if constexpr (useBufferAddressVertex) allocInfo.pNext = &extraInfo;
		allocInfo.allocationSize = vertexByteSize;
		allocInfo.memoryTypeIndex = memoryIndex;
		if (VK_SUCCESS != vkAllocateMemory(vkDevice, &allocInfo, nullptr, &vertexMemory))
		{
			throw std::runtime_error("Failed to vertex/index memory");
		}

		// Bind memory object and image object
		if (VK_SUCCESS != vkBindBufferMemory(vkDevice, vertexBuffer, vertexMemory, 0))
		{
			throw std::runtime_error("Failed to bind vertex buffer memory");
		}
	}

if constexpr (useBufferAddressVertex)
{
	VkBufferDeviceAddressInfo bufferAddressCreateInfo{};
	bufferAddressCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
	bufferAddressCreateInfo.buffer = vertexBuffer;

	vertexBufferAddress = vkGetBufferDeviceAddress(vkDevice, &bufferAddressCreateInfo);

	CreatePushContant("VertexBufferAddress", sizeof(vertexBufferAddress), &vertexBufferAddress);
}
}

template<typename T>
void VkResourceLayout::CreateIndexBuffer(const std::vector<T>& indexData)
{
	indexCount = indexData.empty() ? 0 : indexData.size();
	indexByteSize = indexData.empty() ? 0 : sizeof(T) * indexCount;

	indexBuffer = indexData.empty() ? VK_NULL_HANDLE : CreateBuffer("", indexByteSize, BufferUsage::INDEX, nullptr);

	if (!indexData.empty())
	{
		VkMemoryRequirements indexMemoryRequirements;
		vkGetBufferMemoryRequirements(vkDevice, vertexBuffer, &indexMemoryRequirements);
		// Find Suitable Memory Type
		uint32_t memoryIndex = FindMemoryType(
			vkPhysicalDevice,
			indexMemoryRequirements,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);

		// Allocate Memory
		VkMemoryAllocateInfo allocInfo;
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.pNext = nullptr;
		allocInfo.allocationSize = indexByteSize;
		allocInfo.memoryTypeIndex = memoryIndex;

		if (VK_SUCCESS != vkAllocateMemory(vkDevice, &allocInfo, nullptr, &indexMemory))
		{
			throw std::runtime_error("Failed to vertex/index memory");
		}

		// Bind memory object and image object
		if (VK_SUCCESS != vkBindBufferMemory(vkDevice, indexBuffer, indexMemory, 0))
		{
			throw std::runtime_error("Failed to bind index buffer memory");
		}
	}
}

void VkResourceLayout::CreateMeshData(const Mesh& m)
{
	auto vertexData = m.GetVertexData();
	CreateVertexBuffer(vertexData);

	auto indexData = m.GetIndexData();
	CreateIndexBuffer(indexData);

	//-------------Stage Buffer/Memory Creation
	stageBuffer = CreateBuffer("", vertexByteSize + indexByteSize, BufferUsage::STAGING, nullptr);

	VkMemoryRequirements stageMemoryRequirements;
	vkGetBufferMemoryRequirements(vkDevice, stageBuffer, &stageMemoryRequirements);

	uint32_t stageMemoryIndex = FindMemoryType(
		vkPhysicalDevice,
		stageMemoryRequirements,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
	);

	// Allocate Memory
	VkMemoryAllocateInfo stageAllocInfo;
	stageAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	stageAllocInfo.pNext = nullptr;
	stageAllocInfo.allocationSize = vertexByteSize + indexByteSize;
	stageAllocInfo.memoryTypeIndex = stageMemoryIndex;

	if (VK_SUCCESS != vkAllocateMemory(vkDevice, &stageAllocInfo, nullptr, &stageMemory))
	{
		throw std::runtime_error("Failed to create stage buffer");
	}

	// Stage Mapping
	{
		void* mappedData;
		if (VK_SUCCESS != vkMapMemory(vkDevice, stageMemory, 0, VK_WHOLE_SIZE, 0, &mappedData))
		{
			throw std::runtime_error("Failed to map stage Memory");
		}

		std::memcpy(mappedData, vertexData.data(), vertexByteSize);
		if(indexByteSize) std::memcpy((char*)mappedData + vertexByteSize, indexData.data(), indexByteSize);

		vkUnmapMemory(vkDevice, stageMemory);

		// Bind memory object and image object
		if (VK_SUCCESS != vkBindBufferMemory(vkDevice, stageBuffer, stageMemory, 0))
		{
			throw std::runtime_error("Failed to bind stage buffer memory");
		}
	}

}

void VkResourceLayout::CreatePushContant(const std::string& name, uint32_t size, void* initData)
{
	// 一个Resouce对应一个ResourceInfo
	if (FindResource(name)) return;

	resourceInfo.emplace_back(name, VK_DESCRIPTOR_TYPE_MAX_ENUM, VK_SHADER_STAGE_ALL, size, size);
	pushConstants.emplace_back(name, initData);

}

void VkResourceLayout::CreateConstantBuffer(const std::string& name, uint32_t stride, uint32_t size, void* initData)
{
	resourceNames.push_back(name);
	poolMembers[VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER]++;

	// 一个Resouce对应一个ResourceInfo
	if (FindResource(name)) return;

	resourceInfo.emplace_back(name, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, stride, size);
	CreateBuffer(name, size, BufferUsage::SRV, initData, true);
}

void VkResourceLayout::CreateSRV(const std::string& name, const char* filePath)
{
	resourceNames.push_back(name);
	poolMembers[VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER]++;

	// 一个Resouce对应一个ResourceInfo
	if (FindResource(name)) return;

	// TODO: 写死的!写死的!
	CreateTestImage(name, filePath);

	resourceInfo.emplace_back(name, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 0, testImageWidth * testImageHeight * 4);
}

void* VkResourceLayout::SetPushConstant(uint32_t i, VkShaderStageFlags& stage, uint32_t& size)
{
	if (auto res = FindResource(pushConstants[i].name))
	{
		stage = res->stage;
		size = res->size;
	} else std::runtime_error("Something went wrong when configuring push constants");
	return pushConstants[i].data;
}

void VkResourceLayout::AllocateDescriptorSet()
{
	VkDescriptorSetAllocateInfo allocInfo;
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.pNext = nullptr;
	allocInfo.descriptorPool = descriptorPool;
	allocInfo.descriptorSetCount = 1;
	allocInfo.pSetLayouts = &setLayout;

	if (VK_SUCCESS != vkAllocateDescriptorSets(vkDevice, &allocInfo, &set))
	{
		throw std::runtime_error("Failed to create descriptor set");
	}
}

void VkResourceLayout::BindResourcesAndDescriptors()
{
	std::vector<VkWriteDescriptorSet> writeDescriptors;
	// Buffer
	for (uint32_t i = 0; i < resourceNames.size(); i++)
	{
		if (auto res = FindResource(resourceNames[i]))
		{
			VkWriteDescriptorSet writeDescriptor;
			writeDescriptor.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			writeDescriptor.pNext = nullptr;
			writeDescriptor.dstSet = set;
			writeDescriptor.dstBinding = i;
			writeDescriptor.dstArrayElement = 0; // For array
			writeDescriptor.descriptorCount = 1; // For array
			writeDescriptor.descriptorType = res->type;
			VkDescriptorBufferInfo bufferInfo;
			for (auto& b : buffers) if (res->name == b.name)
			{
				bufferInfo.buffer = std::get<VkBuffer>(b.res);
				bufferInfo.offset = 0;
				bufferInfo.range = b.size;
				writeDescriptor.pBufferInfo = &bufferInfo;
			}

			VkDescriptorImageInfo imageInfo;
			for (auto& i : Images) if (res->name == i.name)
			{
				//TODO: 写死写死....
				imageInfo.sampler		= CreateTestImageSampler();
				imageInfo.imageView		= CreateTestImageView(std::get<VkImage>(i.res), VK_FORMAT_R8G8B8A8_SRGB);
				imageInfo.imageLayout	= VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				writeDescriptor.pImageInfo = &imageInfo;
			}
			
			writeDescriptors.push_back(std::move(writeDescriptor));
		}
	}
	////Image
	//VkDescriptorImageInfo srcImageInfo = { srcSampler , srcImageView, VK_IMAGE_LAYOUT_GENERAL };
	//VkWriteDescriptorSet writeDescriptor;
	//writeDescriptor.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	//writeDescriptor.pNext = 0;
	//writeDescriptor.dstSet = set;
	//writeDescriptor.dstBinding = ?;
	//writeDescriptor.dstArrayElement = 0;
	//writeDescriptor.descriptorCount = ?;
	//writeDescriptor.descriptorType = ?;

	//writeDescriptor.pImageInfo = &srcImageInfo;

	//writeDescriptors.push_back(std::move(writeDescriptor));

	vkUpdateDescriptorSets(vkDevice, writeDescriptors.size(), writeDescriptors.data(), 0, nullptr);
}

VkPipelineLayout VkResourceLayout::BuildPipelineLayout()
{
	std::vector<VkDescriptorSetLayoutBinding> bindings{};
	for (uint32_t i = 0; i < resourceNames.size(); i++)
	{		
		if (auto res = FindResource(resourceNames[i]))
		{
			VkDescriptorSetLayoutBinding binding;
			binding.binding = i;
			binding.descriptorType = res->type;
			binding.descriptorCount = 1;
			binding.stageFlags = res->stage;
			binding.pImmutableSamplers = nullptr;
			bindings.push_back(std::move(binding));
		}
	}

	{
		VkDescriptorSetLayoutCreateInfo createInfo;
		createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		createInfo.pNext = nullptr;
		createInfo.flags = 0;
		createInfo.bindingCount = bindings.size();
		createInfo.pBindings = bindings.data();

		if (VK_SUCCESS != vkCreateDescriptorSetLayout(vkDevice, &createInfo, nullptr, &setLayout))
		{
			throw std::runtime_error("Failed to create descriptor set layout");
		}
	}

	{
		std::vector<VkPushConstantRange> pushConstRanges;
		for (auto& entry : pushConstants)
		{
			uint32_t offset = 0;
			if (auto res = FindResource(entry.name))
			{
				pushConstRanges.emplace_back(
					res->stage,
					offset,
					res->size
				);
				offset += res->size;
			}
		}
		// TODO: check total size of push contants. Shouldn't exceed maxPushConstantsSize

		VkPipelineLayoutCreateInfo createInfo;
		createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		createInfo.pNext = nullptr;
		createInfo.flags = 0;
		createInfo.setLayoutCount = 1;
		createInfo.pSetLayouts = &setLayout;
		createInfo.pushConstantRangeCount = pushConstRanges.size();
		createInfo.pPushConstantRanges = pushConstRanges.data();

		if (VK_SUCCESS != vkCreatePipelineLayout(vkDevice, &createInfo, nullptr, &pipelineLayout))
		{
			throw std::runtime_error("Failed to create Graphics Pipeline");
		}
	}
	return pipelineLayout;
}

uint32_t VkResourceLayout::FindMemoryType(VkPhysicalDevice pDev, const VkMemoryRequirements& memoryRequirements, VkMemoryPropertyFlags requiredFlags, VkMemoryPropertyFlags preferredFlags)
{
	VkPhysicalDeviceMemoryProperties memoryProperties;
	vkGetPhysicalDeviceMemoryProperties(pDev, &memoryProperties);

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

void VkResourceLayout::CreateDescriptorPool(VkDevice vkDevice)
{
	std::vector<VkDescriptorPoolSize> poolContent;
	for (auto& entry : poolMembers)
	{
		VkDescriptorPoolSize poolType;
		poolType.type = entry.first;
		poolType.descriptorCount = entry.second;
		poolContent.push_back(std::move(poolType));
	}

	VkDescriptorPoolCreateInfo createInfo;
	createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.flags = 0;
	createInfo.maxSets = 10u; // TODO: Hardcoded size
	// Only specified descriptor type may be allocated from the pool
	createInfo.poolSizeCount = poolContent.size();
	createInfo.pPoolSizes = poolContent.data();

	if (VK_SUCCESS != vkCreateDescriptorPool(vkDevice, &createInfo, nullptr, &descriptorPool))
	{
		throw std::runtime_error("Failed to create descriptor pool");
	}
}

VkBuffer VkResourceLayout::CreateBuffer(const std::string& name, uint32_t size, BufferUsage usage, void* data, bool Register)
{
	VkBufferCreateInfo createInfo;
	createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.flags = 0;
	createInfo.size = size;
	createInfo.usage = ConvertToVulkanBufferUsage(usage);
	createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	createInfo.queueFamilyIndexCount = 0; //ignored
	createInfo.pQueueFamilyIndices = nullptr;

	VkBuffer buffer;
	if (VK_SUCCESS != vkCreateBuffer(vkDevice, &createInfo, nullptr, &buffer))
	{
		throw std::runtime_error("Failed to Vertex Buffer");
	}

	if (Register)
	{
		buffers.emplace_back(name, size, buffer, data);

		VkMemoryRequirements memoryRequirements;
		vkGetBufferMemoryRequirements(vkDevice, buffer, &memoryRequirements);

		bufferMemoryRequirements.memoryTypeBits |= memoryRequirements.memoryTypeBits;
	}

	return buffer;
}

VkBufferUsageFlags VkResourceLayout::ConvertToVulkanBufferUsage(BufferUsage usage)
{
	switch (usage) {
	case BufferUsage::UAV:			{return VK_BUFFER_USAGE_STORAGE_BUFFER_BIT; }
	case BufferUsage::SRV:			{return VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT; }
	case BufferUsage::VERTEX:		{return VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT; }
	case BufferUsage::INDEX:		{return VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT; }
	case BufferUsage::STAGING:		{return VK_BUFFER_USAGE_TRANSFER_SRC_BIT; }
	case BufferUsage::BUFFERADDR:	{return VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT; }
	default:
	{
		throw std::runtime_error("Invalid Buffer Usage specified!");
	}
	}
	return {};
}

void VkResourceLayout::CreateTestImage(const std::string& name, const char* filePath)
{
	// Load image source data
	int texWidth, texHeight, texChannels;
	stbi_uc* pixels = stbi_load(filePath, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
	uint32_t imageSize = texWidth * texHeight * 4;

	testImageWidth = texWidth;
	testImageHeight = texHeight;

	if (!pixels) {
		throw std::runtime_error("failed to load texture image!");
	}

	//-------------Stage Buffer/Memory Creation
	stageBufferForImage = CreateBuffer("", imageSize, BufferUsage::STAGING, nullptr);

	VkMemoryRequirements stageMemoryRequirements;
	vkGetBufferMemoryRequirements(vkDevice, stageBuffer, &stageMemoryRequirements);

	uint32_t stageMemoryIndex = FindMemoryType(
		vkPhysicalDevice,
		stageMemoryRequirements,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
	);

	// Allocate Memory
	VkMemoryAllocateInfo stageAllocInfo;
	stageAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	stageAllocInfo.pNext = nullptr;
	stageAllocInfo.allocationSize = imageSize;
	stageAllocInfo.memoryTypeIndex = stageMemoryIndex;

	if (VK_SUCCESS != vkAllocateMemory(vkDevice, &stageAllocInfo, nullptr, &stageMemoryForImage))
	{
		throw std::runtime_error("Failed to create stage buffer");
	}

	// Stage Mapping
	{
		void* mappedData;
		if (VK_SUCCESS != vkMapMemory(vkDevice, stageMemoryForImage, 0, VK_WHOLE_SIZE, 0, &mappedData))
		{
			throw std::runtime_error("Failed to map stage Memory");
		}

		std::memcpy(mappedData, pixels, imageSize);

		vkUnmapMemory(vkDevice, stageMemoryForImage);

		// Bind memory object and image object
		if (VK_SUCCESS != vkBindBufferMemory(vkDevice, stageBufferForImage, stageMemoryForImage, 0))
		{
			throw std::runtime_error("Failed to bind stage buffer memory");
		}
	}

	stbi_image_free(pixels);

	testImage = CreateTestImage(name, texWidth, texHeight, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, true);

	{	//Register
		Images.emplace_back(name, imageSize, testImage, nullptr);

		VkMemoryRequirements memoryRequirements;
		vkGetImageMemoryRequirements(vkDevice, testImage, &memoryRequirements);

		imageSRVMemoryRequirements.memoryTypeBits |= memoryRequirements.memoryTypeBits;
	}
}

VkImage VkResourceLayout::CreateTestImage(const std::string& name, uint32_t width, uint32_t height, VkFormat format, VkImageUsageFlags usage, bool SRV)
{
	VkImageCreateInfo imageCreateInfo{};
	imageCreateInfo.sType						= VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageCreateInfo.pNext						= nullptr;
	imageCreateInfo.flags						= 0;
	imageCreateInfo.imageType					= VK_IMAGE_TYPE_2D;
	imageCreateInfo.format						= format;
	imageCreateInfo.extent						= { width , height, 1 };
	imageCreateInfo.mipLevels					= 1;
	imageCreateInfo.arrayLayers					= 1;
	imageCreateInfo.samples						= VK_SAMPLE_COUNT_1_BIT;
	imageCreateInfo.tiling						= SRV ? VK_IMAGE_TILING_OPTIMAL : VK_IMAGE_TILING_LINEAR; //SRV, UAV uses VK_IMAGE_TILING_LINEAR
	imageCreateInfo.usage						= usage;
	imageCreateInfo.sharingMode					= VK_SHARING_MODE_EXCLUSIVE;
	imageCreateInfo.queueFamilyIndexCount		= 0;		//Ignored
	imageCreateInfo.pQueueFamilyIndices			= nullptr;	//Ignored
	imageCreateInfo.initialLayout				= VK_IMAGE_LAYOUT_UNDEFINED;

	VkImage image;
	if (vkCreateImage(vkDevice, &imageCreateInfo, nullptr, &image) != VK_SUCCESS) {
		throw std::runtime_error("failed to create image!");
	}
	
	return image;
}

VkImageView VkResourceLayout::CreateTestImageView(VkImage image, VkFormat format)
{
	VkImageViewCreateInfo viewInfo{};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = image;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format = format;
	viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;

	VkImageView imageView;
	if (vkCreateImageView(vkDevice, &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
		throw std::runtime_error("failed to create texture image view!");
	}

	return imageView;
}

VkSampler VkResourceLayout::CreateTestImageSampler()
{
	VkSamplerCreateInfo samplerInfo{};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter = VK_FILTER_LINEAR;
	samplerInfo.minFilter = VK_FILTER_LINEAR;
	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.anisotropyEnable = VK_TRUE;
	samplerInfo.maxAnisotropy = 8; // TODO:不要写死, 我的6900XT是16
	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplerInfo.unnormalizedCoordinates = VK_FALSE;
	samplerInfo.compareEnable = VK_FALSE;
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerInfo.mipLodBias = 0.0f;
	samplerInfo.minLod = 0.0f;
	samplerInfo.maxLod = 0.0f;

	VkSampler sampler;
	if (vkCreateSampler(vkDevice, &samplerInfo, nullptr, &sampler) != VK_SUCCESS) {
		throw std::runtime_error("failed to create texture sampler!");
	}
	return sampler;
}

void VkResourceLayout::CreateResources(VkDevice vkDevice, VkPhysicalDevice vkPhysicalDevice)
{
	CreateMemory(vkDevice, vkPhysicalDevice);
	BindBuffersAndMemory(vkDevice);
	BindImagesAndMemory(vkDevice);
}

void VkResourceLayout::CreateMemory(VkDevice vkDevice, VkPhysicalDevice vkPhysicalDevice)
{
	// Buffer Memory Creation
	{
		// Find Suitable Memory Type
		uint32_t memoryIndex = FindMemoryType(
			vkPhysicalDevice,
			bufferMemoryRequirements,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
		);

		uint32_t totalSize = 0;
		for (auto& entry : buffers) totalSize += entry.size;

		// Allocate Memory
		VkMemoryAllocateInfo allocInfo;
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.pNext = nullptr;
		allocInfo.allocationSize = totalSize;
		allocInfo.memoryTypeIndex = memoryIndex;

		if (VK_SUCCESS != vkAllocateMemory(vkDevice, &allocInfo, nullptr, &bufferMemory))
		{
			throw std::runtime_error("Failed to create Buffer Memory");
		}
	}

	// SRV Image Memory Creation
	{
		uint32_t memoryIndex = FindMemoryType(
			vkPhysicalDevice,
			imageSRVMemoryRequirements,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);

		uint32_t totalSize = 0;
		for (auto& entry : Images) totalSize += entry.size;

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.pNext = nullptr;
		allocInfo.allocationSize = totalSize;
		allocInfo.memoryTypeIndex = memoryIndex;

		if (VK_SUCCESS != vkAllocateMemory(vkDevice, &allocInfo, nullptr, &ImageMemory)) {
			throw std::runtime_error("Failed to create image memory!");
		}
	}
}

void VkResourceLayout::BindBuffersAndMemory(VkDevice vkDevice)
{
	uint32_t offset = 0;
	for (auto& entry : buffers)
	{
		// 如果提供了初始数据, 就要初始化一下Buffer
		// TODO: 这是通过CPU与GPU直接Mapping, 不是Staging Buffer, 效率其实不理想, 要改
		if (entry.data)
		{
			void* mappedData;
			if (VK_SUCCESS != vkMapMemory(vkDevice, bufferMemory, offset, entry.size, 0, &mappedData))
			{
				throw std::runtime_error("Failed to Map Memory");
			}

			std::memcpy(mappedData, entry.data, entry.size);
			vkUnmapMemory(vkDevice, bufferMemory);
		}

		// Bind memory object and image object
		if (VK_SUCCESS != vkBindBufferMemory(vkDevice, std::get<VkBuffer>(entry.res), bufferMemory, offset))
		{
			throw std::runtime_error("Failed to bind buffer and memory");
		}
		offset += entry.size;
	}
}

void VkResourceLayout::BindImagesAndMemory(VkDevice vkDevice)
{
	uint32_t offset = 0;
	for (auto& entry : Images)
	{
		// TODO: Image暂时不支持直接Copy Paste数据
		// Bind memory object and image object
		if (VK_SUCCESS != vkBindImageMemory(vkDevice, std::get<VkImage>(entry.res), ImageMemory, offset))
		{
			throw std::runtime_error("Failed to bind buffer and memory");
		}
		offset += entry.size;
	}
}

}