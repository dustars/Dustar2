/*
    Description:
    The implementation of VkRenderResource module

    Created Date:
    5/16/2022 10:47:46 PM
*/

module;
#include <stdexcept>
module VkRenderResource;

namespace RB
{

std::unordered_map<VkDescriptorType, uint32_t> VkResourceLayout::poolMembers;
VkDescriptorPool VkResourceLayout::descriptorPool;
std::vector<VkResourceLayout::ResourceInfo> VkResourceLayout::resourceInfo;
VkMemoryRequirements VkResourceLayout::bufferMemoryRequirements;
VkDeviceMemory VkResourceLayout::bufferMemory;
VkMemoryRequirements VkResourceLayout::imageMemoryRequirements;
VkDeviceMemory VkResourceLayout::ImageMemory;
std::vector<VkResourceLayout::resEntry> VkResourceLayout::buffers;
std::vector<VkResourceLayout::resEntry> VkResourceLayout::Images;

void VkResourceLayout::CreateConstantBuffer(const std::string& name, uint32_t stride, uint32_t size, void* initData)
{
	resourceNames.push_back(name);
	poolMembers[VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER]++;

	// 每个 Resource 只能存在一个entry
	for (auto & entry : resourceInfo) if (entry.name == name) return;

	resourceInfo.emplace_back(name, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, stride, size);

	CreateBuffer(name, size, initData);

	resourceNames.push_back(name);
	poolMembers[VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER]++;
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

void VkResourceLayout::UpdateDescriptorSet()
{
	std::vector<VkWriteDescriptorSet> writeDescriptors;
	// Buffer
	for (uint32_t i = 0; i < resourceNames.size(); i++)
	{
		for (auto& entry : resourceInfo) if (entry.name == resourceNames[i])
		{
			VkWriteDescriptorSet writeDescriptor;
			writeDescriptor.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			writeDescriptor.pNext = nullptr;
			writeDescriptor.dstSet = set;
			writeDescriptor.dstBinding = i;
			writeDescriptor.dstArrayElement = 0; // For array
			writeDescriptor.descriptorCount = 1; // For array
			writeDescriptor.descriptorType = entry.type;
			VkDescriptorBufferInfo bufferInfo;
			for (auto& b : buffers) if (entry.name == b.name)
			{
				bufferInfo.buffer = std::get<VkBuffer>(b.res);
				bufferInfo.offset = 0;
				bufferInfo.range = b.size;
			}
			writeDescriptor.pBufferInfo = &bufferInfo;
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

void VkResourceLayout::BindResourcesAndDescriptors()
{

}

VkPipelineLayout VkResourceLayout::BuildPipelineLayout()
{
	std::vector<VkDescriptorSetLayoutBinding> bindings{};
	for (uint32_t i = 0; i < resourceNames.size(); i++)
	{		
		for (auto& entry : resourceInfo)
		if (entry.name == resourceNames[i])
		{
			VkDescriptorSetLayoutBinding binding;
			binding.binding = i;
			binding.descriptorType = entry.type;
			binding.descriptorCount = 1;
			binding.stageFlags = entry.stage;
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
		VkPipelineLayoutCreateInfo createInfo;
		createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		createInfo.pNext = nullptr;
		createInfo.flags = 0;
		createInfo.setLayoutCount = 1;
		createInfo.pSetLayouts = &setLayout;
		createInfo.pushConstantRangeCount = 0; // TODO
		createInfo.pPushConstantRanges = nullptr; // TODO

		if (VK_SUCCESS != vkCreatePipelineLayout(vkDevice, &createInfo, nullptr, &pipelineLayout))
		{
			throw std::runtime_error("Failed to create Graphics Pipeline");
		}
	}
	return pipelineLayout;
}

void VkResourceLayout::DestroyResource()
{
//	buffers;
//Images;
//ImageMemory
//bufferMemory
//descriptorPool

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
	createInfo.maxSets = 10u;
	// Only specified descriptor type may be allocated from the pool
	createInfo.poolSizeCount = poolContent.size();
	createInfo.pPoolSizes = poolContent.data();

	if (VK_SUCCESS != vkCreateDescriptorPool(vkDevice, &createInfo, nullptr, &descriptorPool))
	{
		throw std::runtime_error("Failed to create descriptor pool");
	}
}

void VkResourceLayout::CreateBuffer(const std::string& name, uint32_t size, void* data)
{
	VkBufferCreateInfo createInfo;
	createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.flags = 0;
	createInfo.size = size;
	createInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
	createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	createInfo.queueFamilyIndexCount = 0; //ignored
	createInfo.pQueueFamilyIndices = nullptr;

	VkBuffer buffer;
	if (VK_SUCCESS != vkCreateBuffer(vkDevice, &createInfo, nullptr, &buffer))
	{
		throw std::runtime_error("Failed to Vertex Buffer");
	}

	buffers.emplace_back( name, size, buffer, data);

	VkMemoryRequirements memoryRequirements;
	vkGetBufferMemoryRequirements(vkDevice, buffer, &memoryRequirements);

	bufferMemoryRequirements.memoryTypeBits |= memoryRequirements.memoryTypeBits;
}

void VkResourceLayout::CreateImage(const std::string&)
{

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

}

void VkResourceLayout::BindBuffersAndMemory(VkDevice vkDevice)
{
	uint32_t offset = 0;
	for (auto& entry : buffers)
	{
		// 如果提供了初始数据, 就要小小地Mapping一下
		if (entry.data != nullptr)
		{
			void* mappedData;
			if (VK_SUCCESS != vkMapMemory(vkDevice, bufferMemory, offset, entry.size, 0, &mappedData))
			{
				throw std::runtime_error("Failed to Map Memory");
			}

			memcpy(mappedData, entry.data, entry.size);
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

}

}