/*
    Description:
    The interface of VkCommandBuffer module

    Created Date:
    5/14/2022 6:36:58 PM

    Notes:
*/

export module VkCmdBuffer;

import <vulkan\vulkan.h>;

namespace RB
{

export class VkCmdBuffer
{
	// Queue and Command Buffers
	VkCommandPool vkCommandPool;
	VkCommandBuffer cmd;

	VkDevice* devicePtr;

public:
	VkCmdBuffer();
	~VkCmdBuffer();

	void InitCommandBuffer(VkDevice*, uint32_t);

	VkCommandBuffer& GetCommandBuffer() { return cmd; }

	void BeginCommandBuffer();
	void EndCommandBuffer();
	void SubmitCommandBuffer(VkQueue, VkSemaphore, VkSemaphore);

	void BeginRenderPass(VkFramebuffer, VkRenderPass, VkExtent2D);
	void EndRenderPass();

	void Draw(VkBuffer, VkPipeline, uint32_t);

private:
	void InitCommandPool(uint32_t);
	void InitCommandBuffers();
};

}