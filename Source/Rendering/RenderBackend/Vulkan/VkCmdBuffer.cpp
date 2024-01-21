/*
    Description:
    The implementation of VkCommandBuffer module

    Created Date:
    5/14/2022 6:37:09 PM
*/

module;
#include <vulkan\vulkan.h>
module VkCmdBuffer;

namespace RB
{

void VkCmdBuffer::InitCommandBuffer(VkDevice* devP, uint32_t queueFamilyIndex)
{
	devicePtr = devP;
	InitCommandPool(queueFamilyIndex);
	InitCommandBuffers();
}

void VkCmdBuffer::DestroyCommandBuffer()
{
	vkDestroyCommandPool(*devicePtr, vkCommandPool, nullptr);
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

void VkCmdBuffer::SubmitCommandBuffer(VkQueue queue, VkFence fence)
{
	VkPipelineStageFlags transferFlag[] = { VK_PIPELINE_STAGE_TRANSFER_BIT };
	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.pNext = nullptr;
	submitInfo.pWaitDstStageMask = transferFlag;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &cmd;

	if (VK_SUCCESS != vkQueueSubmit(queue, 1, &submitInfo, fence))
	{
		throw std::runtime_error("Failed to submit queue");
	}
}

void VkCmdBuffer::BeginRenderPass(const VkGraphicsPipeline& pipeline, uint32_t imageIndex)
{
	VkRect2D area = { {0,0}, pipeline.GetSurfaceRef().GetExtent() };
	VkClearValue clearColor;
	clearColor.color = { 1.0f, 0.f, 0.f, 1.f };
	VkRenderPassBeginInfo beginInfo;
	beginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	beginInfo.pNext = nullptr;
	beginInfo.renderPass = pipeline.GetRenderPass();
	beginInfo.framebuffer = pipeline.GetFrameBuffer(imageIndex);
	beginInfo.renderArea = area;
	beginInfo.clearValueCount = 1;
	beginInfo.pClearValues = &clearColor;

	vkCmdBeginRenderPass(cmd, &beginInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void VkCmdBuffer::EndRenderPass()
{
	vkCmdEndRenderPass(cmd);
}

void VkCmdBuffer::BeginDynamicRendering(VkGraphicsPipeline& pipeline, uint32_t imageIndex)
{
	VkClearValue clearColor{.color = { 0.2f, 0.2f, 0.3f, 1.f } };
	VkRenderingAttachmentInfo colorAttach{
		.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
		.imageView = *pipeline.GetSurfaceRef().GetAvailableImageView(imageIndex),
		.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
		.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
		.clearValue = clearColor
	};

	VkRect2D area = { {0,0}, pipeline.GetSurfaceRef().GetExtent() };

	VkRenderingInfo renderInfo{
	.sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
	.renderArea = area,
	.layerCount = 1,
	.colorAttachmentCount = 1,
	.pColorAttachments = &colorAttach };
	//.pDepthAttachment
	//.pStencilAttachment
	vkCmdBeginRendering(cmd, &renderInfo);
}

void VkCmdBuffer::EndDynamicRendering()
{
	vkCmdEndRendering(cmd);
}

void VkCmdBuffer::ImageTransition(VkImage image, VkImageLayout currentLayout, VkImageLayout newLayout)
{
	VkImageMemoryBarrier2 imageBarrier{ .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2 };
	imageBarrier.pNext = nullptr;

	imageBarrier.srcStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
	imageBarrier.srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT;
	imageBarrier.dstStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
	imageBarrier.dstAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT | VK_ACCESS_2_MEMORY_READ_BIT;

	imageBarrier.oldLayout = currentLayout;
	imageBarrier.newLayout = newLayout;

	VkImageSubresourceRange subImage{};
	subImage.aspectMask = (newLayout == VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL) ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
	subImage.baseMipLevel = 0;
	subImage.levelCount = VK_REMAINING_MIP_LEVELS;
	subImage.baseArrayLayer = 0;
	subImage.layerCount = VK_REMAINING_ARRAY_LAYERS;

	imageBarrier.subresourceRange = subImage;
	imageBarrier.image = image;

	VkDependencyInfo depInfo{};
	depInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
	depInfo.pNext = nullptr;

	depInfo.imageMemoryBarrierCount = 1;
	depInfo.pImageMemoryBarriers = &imageBarrier;

	vkCmdPipelineBarrier2(cmd, &depInfo);
}

void VkCmdBuffer::Draw(Pipeline& pipeline)
{
	//TODO: 形参就该是VkGraphicsPipeline...现在这样不合理
	VkGraphicsPipeline& p = dynamic_cast<VkGraphicsPipeline&>(pipeline);

	vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, p.GetPipeline());

	uint32_t offset = 0;
	for (uint32_t i = 0; i < p.GetPushConstantsSize(); i++)
	{
		VkShaderStageFlags stage;
		uint32_t size;
		if (void* data = p.SetPushConstant(i, stage, size))
		{
			vkCmdPushConstants(cmd, p.GetPipelineLayout(), stage, offset, size, data);
		}
		else throw std::runtime_error("Push constant value is invalid. Please check if given data has been destroyed");
		offset += size;
	}

	auto* setPtr = p.GetDescriptorSet();
	vkCmdBindDescriptorSets(
		cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, p.GetPipelineLayout(),
		0, 1, setPtr, // Descriptor
		0, nullptr);

	VkDeviceSize offsets[] = { 0 };
if constexpr (!useBufferAddressVertex)
{
	vkCmdBindVertexBuffers(cmd, 0, 1, p.GetVertexBufferPtr(), offsets);
}
	const VkBuffer* indexBuffer = p.GetIndexBufferPtr();
	if (*indexBuffer != VK_NULL_HANDLE) // Draw Indexed
	{
		vkCmdBindIndexBuffer(cmd, *indexBuffer, offsets[0], VK_INDEX_TYPE_UINT16);
		vkCmdDrawIndexed(cmd, p.GetIndexCount(), 1, 0, 0, 0);
	}
	else // Draw
	{
		vkCmdDraw(cmd, p.GetVertexCount(), 1, 0, 0);
	}
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