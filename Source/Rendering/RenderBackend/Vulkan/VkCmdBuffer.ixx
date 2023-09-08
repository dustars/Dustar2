/*
    Description:
    The interface of VkCommandBuffer module

    Created Date:
    5/14/2022 6:36:58 PM

    Notes:
*/

export module VkCmdBuffer;

import std;
import CmdBuffer;
import Pipeline;
import VkGraphicsPipeline;
import VkComputePipeline;
import <vulkan\vulkan.h>;

namespace RB
{

export class VkCmdBuffer : public CmdBuffer
{
	// Queue and Command Buffers
	VkCommandPool vkCommandPool;
	VkCommandBuffer cmd;

	VkDevice* devicePtr;

public:
	void InitCommandBuffer(VkDevice*, uint32_t);
	void DestroyCommandBuffer();

	VkCommandBuffer& GetCommandBuffer() { return cmd; }

	void BeginCommandBuffer();
	void EndCommandBuffer();
	void SubmitCommandBuffer(VkQueue, VkSemaphore, VkSemaphore);

	void BeginRenderPass(const VkGraphicsPipeline&, uint32_t);
	void EndRenderPass();

	virtual void Draw(Pipeline&) final override;

private:
	void InitCommandPool(uint32_t);
	void InitCommandBuffers();
};

}