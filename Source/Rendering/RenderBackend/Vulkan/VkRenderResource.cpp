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

VkDescriptorPool VkResourceLayout::descriptorPool = VK_NULL_HANDLE;

void VkResourceLayout::CreateConstantBuffer()
{
	VkDescriptorSetLayoutBinding binding;
	binding.binding = bindings.size();
	binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	binding.descriptorCount = 1;
	binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	binding.pImmutableSamplers = nullptr;
	bindings.push_back(std::move(binding));
}

VkPipelineLayout VkResourceLayout::BuildPipelineLayout(VkDevice vkDevice)
{
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

}