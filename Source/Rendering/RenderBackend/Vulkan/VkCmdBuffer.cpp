/*
    Description:
    The implementation of VkCommandBuffer module

    Created Date:
    5/14/2022 6:37:09 PM
*/

module;
#include <stdexcept>
#include <vulkan\vulkan.h>
module VkCmdBuffer;

namespace RB
{

VkCmdBuffer::VkCmdBuffer() {}

VkCmdBuffer::~VkCmdBuffer()
{
	vkDestroyCommandPool(*devicePtr, vkCommandPool, nullptr);
}

void VkCmdBuffer::InitCommandBuffer(VkDevice* devP, uint32_t queueFamilyIndex)
{
	devicePtr = devP;
	InitCommandPool(queueFamilyIndex);
	InitCommandBuffers();
}

void VkCmdBuffer::BeginCommandBuffer()
{
	VkCommandBufferBeginInfo beginInfo;
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.pNext = nullptr;
	beginInfo.flags = 0;
	beginInfo.pInheritanceInfo = nullptr;

	if (VK_SUCCESS != vkBeginCommandBuffer(cmd, &beginInfo))
	{
		throw std::runtime_error("Failed to begin Buffer");
	}
}

void VkCmdBuffer::EndCommandBuffer()
{
	if (VK_SUCCESS != vkEndCommandBuffer(cmd))
	{
		throw std::runtime_error("Failed to end Buffer");
	}
}

void VkCmdBuffer::SubmitCommandBuffer(VkQueue queue, VkSemaphore waitS, VkSemaphore signS)
{
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

	VkSubmitInfo submitInfo;
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.pNext = nullptr;
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = &waitS;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &cmd;
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = &signS;

	if (VK_SUCCESS != vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE))
	{
		throw std::runtime_error("Failed to submit queue");
	}
}

void VkCmdBuffer::BeginRenderPass(VkFramebuffer fb, VkRenderPass renderPass, VkExtent2D extent)
{
	VkRect2D area = { {0,0},extent };
	VkClearValue clearColor;
	clearColor.color = { 1.0f, 0.f, 0.f, 1.f };
	VkRenderPassBeginInfo beginInfo;
	beginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	beginInfo.pNext = nullptr;
	beginInfo.renderPass = renderPass;
	beginInfo.framebuffer = fb;
	beginInfo.renderArea = area;
	beginInfo.clearValueCount = 1;
	beginInfo.pClearValues = &clearColor;

	vkCmdBeginRenderPass(cmd, &beginInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void VkCmdBuffer::EndRenderPass()
{
	vkCmdEndRenderPass(cmd);
}

void VkCmdBuffer::Draw(VkBuffer vb, VkPipeline pipeline, uint32_t size)
{
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(cmd, 0, 1, &vb, offsets);

	vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

	vkCmdDraw(cmd, size, 1, 0, 0);
}

void VkCmdBuffer::InitCommandPool(uint32_t queueFamilyIndex)
{
	VkCommandPoolCreateInfo createInfo;
	createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	createInfo.queueFamilyIndex = queueFamilyIndex;

	if (VK_SUCCESS != vkCreateCommandPool(*devicePtr, &createInfo, nullptr, &vkCommandPool))
	{
		throw std::runtime_error("Failed to create command pool");
	}
}

void VkCmdBuffer::InitCommandBuffers()
{
	VkCommandBufferAllocateInfo allocInfo;
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.pNext = nullptr;
	allocInfo.commandPool = vkCommandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = 1;

	if (VK_SUCCESS != vkAllocateCommandBuffers(*devicePtr, &allocInfo, &cmd))
	{
		throw std::runtime_error("Failed to create command buffer");
	}
	// There is no need to explicitly free all command buffers
	// Destroy the Command Pool will also free all cmd buffers as well as associated resources.
}

}