/*
    Description:
    The implementation of VkRenderingBackend module

    Created Date:
    5/10/2022 8:50:08 PM
*/

module;
#define WINDOW_APP
#define VK_USE_PLATFORM_WIN32_KHR

#include <fstream>
#include <stdexcept>
module VkRenderingBackend;

import <vector>;
import <vulkan\vulkan.h>;


namespace RB
{

VkRBInterface::VkRBInterface()
{
	InitVulkanInstance();
	InitVulkanPhysicalDevices();
	InitVulkanLogicalDevice();
	InitVulkanQueues();
	InitVulkanSynchronizations();
	surface = new VkSurface();
	cmd = new VkCmdBuffer();
	surface->InitSurface(&vkInstance, &vkPhysicalDevice, &vkDevice, currentQueueFamilyIndex);
	cmd->InitCommandBuffer(&vkDevice, currentQueueFamilyIndex);

	CreateVertexBuffer();
	CreateRenderPass();
	CreateFramebuffer();
	CreateGraphicsPipeline();
}

VkRBInterface::~VkRBInterface()
{
	vkDestroyPipeline(vkDevice, graphicsPipeline, nullptr);
	vkDestroyPipelineLayout(vkDevice, graphicsPipelineLayout, nullptr);
	vkDestroyShaderModule(vkDevice, vertShader, nullptr);
	vkDestroyShaderModule(vkDevice, fragShader, nullptr);
	for (size_t i = 0; i < framebuffers.size(); i++)
	{
		vkDestroyFramebuffer(vkDevice, framebuffers[i], nullptr);
	}
	vkDestroyRenderPass(vkDevice, renderPass, nullptr);
	vkFreeMemory(vkDevice, vertexMemory, nullptr);
	vkDestroyBuffer(vkDevice, vertexBuffer, nullptr);

	delete cmd;
	delete surface;
	vkDestroySemaphore(vkDevice, imageAvailableSemaphore, nullptr);
	vkDestroySemaphore(vkDevice, renderFinishedSemaphore, nullptr);
	vkDestroyFence(vkDevice, inFlightFence, nullptr);
	vkDestroyDevice(vkDevice, nullptr);
	vkDestroyInstance(vkInstance, nullptr);
}

bool VkRBInterface::Update()
{
	return surface->Update();
}

bool VkRBInterface::Render()
{
	//	for (each lambda)
	//	{
	//		if (RenderPass)
	//			for(each subpass)
	//				lambda()
	//		if (Compute Pass)
	//			lambda()
	//	}
	vkWaitForFences(vkDevice, 1, &inFlightFence, VK_TRUE, UINT64_MAX);
	vkResetFences(vkDevice, 1, &inFlightFence);

	uint32_t imageIndex = surface->GetAvailableImageIndex(UINT64_MAX, imageAvailableSemaphore);
	void BeginRenderPass(VkFramebuffer, VkRenderPass, VkExtent2D);
	void EndRenderPass();

	void Draw(VkBuffer, VkPipeline, uint32_t);
	cmd->BeginCommandBuffer();
	cmd->BeginRenderPass(framebuffers[imageIndex], renderPass, surface->GetExtent());
	cmd->Draw(vertexBuffer, graphicsPipeline, vertexData.size());
	cmd->EndRenderPass();
	cmd->EndCommandBuffer();
	cmd->SubmitCommandBuffer(vkQueues[0], imageAvailableSemaphore, renderFinishedSemaphore);

	WindowPresentation(imageIndex);

	// 相当重要……
	vkQueueWaitIdle(vkQueues[0]);

	return true;
}

void VkRBInterface::InitVulkanInstance()
{
	// 既然可以在创建Instance之前先获取Instance级别的Layer和Extensions,
	// 那是否代表着这两种东西是跟硬件无关的？取决于Vulkan在平台上的实现？
	std::vector<const char*> enabledInstanceLayers;
	EnableInstanceLayers(enabledInstanceLayers);

	std::vector<const char*> enabledInstanceExtensions;
	EnableInstanceExtensions(enabledInstanceExtensions);

	VkApplicationInfo appInfo;
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pNext = nullptr;
	appInfo.pApplicationName = "VulkanLearning";
	appInfo.applicationVersion = 1;
	appInfo.pEngineName = "NoEngine";
	appInfo.engineVersion = 0;
	// 操 这个apiVersion还挺复杂的 建议看文档……
	appInfo.apiVersion = 0;

	VkInstanceCreateInfo createInfo;
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.flags = 0;
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledLayerCount = static_cast<uint32_t>(enabledInstanceLayers.size());
	createInfo.ppEnabledLayerNames = enabledInstanceLayers.data();
	createInfo.enabledExtensionCount = static_cast<uint32_t>(enabledInstanceExtensions.size());
	createInfo.ppEnabledExtensionNames = enabledInstanceExtensions.data();

	if (VK_SUCCESS != vkCreateInstance(&createInfo, nullptr, &vkInstance))
	{
		throw std::runtime_error("failed to Create Vulkan Instance");
	}
}
void VkRBInterface::InitVulkanPhysicalDevices()
{
	uint32_t numPhysicalDevices = 0;

	if (VK_SUCCESS != vkEnumeratePhysicalDevices(vkInstance, &numPhysicalDevices, nullptr))
	{
		throw std::runtime_error("Failed to enumerate physical devices");
	}

	std::vector<VkPhysicalDevice> devices;
	devices.resize(numPhysicalDevices);
	vkEnumeratePhysicalDevices(vkInstance, &numPhysicalDevices, devices.data());
	vkPhysicalDevice = devices[0];

	// TODO: 
	// 1. Make a function to choose the best device if there's any.
	// 2. Find a way to save the physical devices info below,
	//    we'll need a way to determine some run-time behaviors
	//    based on these info.

	// Get physical device properties.
	VkPhysicalDeviceProperties properties;
	vkGetPhysicalDeviceProperties(vkPhysicalDevice, &properties);

	// Get physical device features.
	VkPhysicalDeviceFeatures features;
	vkGetPhysicalDeviceFeatures(vkPhysicalDevice, &features);

	// Get physical device memory properties.
	VkPhysicalDeviceMemoryProperties memoryProperties;
	vkGetPhysicalDeviceMemoryProperties(vkPhysicalDevice, &memoryProperties);
}
void VkRBInterface::InitVulkanLogicalDevice()
{
	// 太多Hardcoded了……未来慢慢丰富吧
	// Queue Family Index默认是0
	// Physical Device默认是0（目前我电脑也只有一个……也就是6900xt）
	std::vector<const char*> enabledDeviceLayers;
	EnableDeviceLayers(enabledDeviceLayers);

	std::vector<const char*> enabledDeviceExtensions;
	EnableDeviceExtensions(enabledDeviceExtensions);

	if (!FindSubitableQueueFamily())
	{
		throw std::runtime_error("Cannot find suitable queue!");
	}

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfo;
	float priorities[1] = { 0 };
	queueCreateInfo.emplace_back(
		VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
		nullptr,
		0,
		currentQueueFamilyIndex,
		1,
		priorities
	);

	VkDeviceCreateInfo createInfo;
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.flags = 0;
	// TODO: Queue的动态选择和创建
	createInfo.pQueueCreateInfos = queueCreateInfo.data();
	createInfo.queueCreateInfoCount = queueCreateInfo.size();
	// TODO: layer和extension的动态加载
	createInfo.enabledLayerCount = static_cast<uint32_t>(enabledDeviceLayers.size());
	createInfo.ppEnabledLayerNames = enabledDeviceLayers.data();
	createInfo.enabledExtensionCount = static_cast<uint32_t>(enabledDeviceExtensions.size());
	createInfo.ppEnabledExtensionNames = enabledDeviceExtensions.data();
	// TODO: 可以把Physical Device创建的feature直接拿来用,！但会增加性能开销！
	// 一个更加自然的实现就是用到什么就enable什么feature,但这个就比较复杂了……
	createInfo.pEnabledFeatures = nullptr;

	if (VK_SUCCESS != vkCreateDevice(vkPhysicalDevice, &createInfo, nullptr, &vkDevice))
	{
		throw std::runtime_error("Failed to create logical device");
	}
}

void VkRBInterface::InitVulkanQueues()
{
	vkQueues.resize(1);
	vkGetDeviceQueue(vkDevice, currentQueueFamilyIndex, 0, &vkQueues[0]);
	if (vkQueues[0] == VK_NULL_HANDLE)
	{
		throw std::runtime_error("Failed to Get Queue");
	}
}

void VkRBInterface::InitVulkanSynchronizations()
{
	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
	if (vkCreateSemaphore(vkDevice, &semaphoreInfo, nullptr, &imageAvailableSemaphore) != VK_SUCCESS ||
		vkCreateSemaphore(vkDevice, &semaphoreInfo, nullptr, &renderFinishedSemaphore) != VK_SUCCESS ||
		vkCreateFence(vkDevice, &fenceInfo, nullptr, &inFlightFence) != VK_SUCCESS) {
		throw std::runtime_error("failed to create semaphores!");
	}
}

void VkRBInterface::EnableInstanceLayers(std::vector<const char*>& enabledInstanceLayers)
{
	//TODO: 后期会需要一个机制在创建APP前确定需要哪些layers & extensions
#ifndef NDEBUG
	enabledInstanceLayers.push_back("VK_LAYER_KHRONOS_validation");
#endif

	uint32_t layerCount;
	std::vector<VkLayerProperties> layers;

	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
	if (layerCount != 0)
	{
		layers.resize(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, layers.data());
	}

	// TODO：获取到该Platform支持的Layers后,要检查是否覆盖了APP所需
	// CheckIfRequiredLayersAvailable()
}

void VkRBInterface::EnableInstanceExtensions(std::vector<const char*>& enabledInstanceExtension)
{
	// TODO: Full screen / Borderless window
#ifdef WINDOW_APP
	enabledInstanceExtension.push_back("VK_KHR_surface");

#ifdef VK_USE_PLATFORM_WIN32_KHR
	enabledInstanceExtension.push_back("VK_KHR_win32_surface");
#endif

#endif

	uint32_t extensionCount;
	std::vector<VkExtensionProperties> extensions;

	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
	if (extensionCount != 0)
	{
		extensions.resize(extensionCount);
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
	}

	// TODO：获取到该Platform支持的Extensions后,要检查是否覆盖了APP所需
	// CheckIfRequiredExtentsionAvailable()
}

void VkRBInterface::EnableDeviceLayers(std::vector<const char*>& enabledDeviceLayers)
{
	uint32_t layerCount;
	std::vector<VkLayerProperties> layers;

	vkEnumerateDeviceLayerProperties(vkPhysicalDevice, &layerCount, nullptr);
	if (layerCount != 0)
	{
		layers.resize(layerCount);
		vkEnumerateDeviceLayerProperties(vkPhysicalDevice, &layerCount, layers.data());
	}
}

void VkRBInterface::EnableDeviceExtensions(std::vector<const char*>& enabledDeviceExtensions)
{
#ifdef WINDOW_APP
	enabledDeviceExtensions.push_back("VK_KHR_swapchain");
#endif

	uint32_t extensionCount;
	std::vector<VkExtensionProperties> extensions;

	vkEnumerateDeviceExtensionProperties(vkPhysicalDevice, nullptr, &extensionCount, nullptr);
	if (extensionCount != 0)
	{
		extensions.resize(extensionCount);
		vkEnumerateDeviceExtensionProperties(vkPhysicalDevice, nullptr, &extensionCount, extensions.data());
	}
}

bool VkRBInterface::FindSubitableQueueFamily()
{
	// Get queue family properties of the physical device
	uint32_t queueFamilyCount;
	vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, &queueFamilyCount, queueFamilyProperties.data());

	currentQueueFamilyIndex = 0;
	for (auto& entry : queueFamilyProperties)
	{
		return entry.queueFlags & (VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT);
		currentQueueFamilyIndex++;
	}
	return false;
}

void VkRBInterface::WindowPresentation(uint32_t imageIndex)
{
	VkResult result;
	VkPresentInfoKHR presentInfo;
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.pNext = nullptr;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &renderFinishedSemaphore;
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &surface->GetSwapChain();
	presentInfo.pImageIndices = &imageIndex;
	presentInfo.pResults = &result;

	if (VK_SUCCESS != vkQueuePresentKHR(vkQueues[0], &presentInfo))
	{
		throw std::runtime_error("Failed to present the Image");
	}

	if (VK_SUCCESS != result)
	{
		throw std::runtime_error("Presentation has gone wrong!");
	}
}

void VkRBInterface::CreateShaderModule(const std::string& shaderFile, VkShaderModule& mod)
{
	std::ifstream file(shaderFile, std::ios::ate | std::ios::binary);

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

	if (VK_SUCCESS != vkCreateShaderModule(vkDevice, &createInfo, nullptr, &mod))
	{
		throw std::runtime_error("Failed to create shader module");
	}
}

void VkRBInterface::CreateVertexBuffer()
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

	if (VK_SUCCESS != vkCreateBuffer(vkDevice, &createInfo, nullptr, &vertexBuffer))
	{
		throw std::runtime_error("Failed to Vertex Buffer");
	}

	// Check Requirement
	VkMemoryRequirements memoryRequirements;
	vkGetBufferMemoryRequirements(vkDevice, vertexBuffer, &memoryRequirements);

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

	if (VK_SUCCESS != vkAllocateMemory(vkDevice, &allocInfo, nullptr, &vertexMemory))
	{
		throw std::runtime_error("Failed to create device memory");
	}

	{ // Mapping
		void* mappedData;
		if (VK_SUCCESS != vkMapMemory(vkDevice, vertexMemory, 0, VK_WHOLE_SIZE, 0, &mappedData))
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
		vkFlushMappedMemoryRanges(vkDevice, 1, &range);

		vkUnmapMemory(vkDevice, vertexMemory);
	}

	// Bind memory object and image object
	if (VK_SUCCESS != vkBindBufferMemory(vkDevice, vertexBuffer, vertexMemory, 0))
	{
		throw std::runtime_error("Failed to bind buffer and memory");
	}
}

void VkRBInterface::CreateRenderPass()
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

	if (VK_SUCCESS != vkCreateRenderPass(vkDevice, &createInfo, nullptr, &renderPass))
	{
		throw std::runtime_error("Failed to create Render Pass");
	}
}

void VkRBInterface::CreateFramebuffer()
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

		if (VK_SUCCESS != vkCreateFramebuffer(vkDevice, &createInfo, nullptr, &framebuffers[i]))
		{
			throw std::runtime_error("Failed to create Framebuffer");
		}
	}
}

void VkRBInterface::CreateGraphicsPipeline()
{
	CreateShaderModule("../Rendering/Shaders/SimpleVertexShader.spv", vertShader);
	CreateShaderModule("../Rendering/Shaders/SimpleFragmentShader.spv", fragShader);
	std::vector<VkPipelineShaderStageCreateInfo> shaderCreateInfo;
	shaderCreateInfo.emplace_back(
		VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
		nullptr,
		0,
		VK_SHADER_STAGE_VERTEX_BIT,
		vertShader,
		"main",
		nullptr
	);

	shaderCreateInfo.emplace_back(
		VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
		nullptr,
		0,
		VK_SHADER_STAGE_FRAGMENT_BIT,
		fragShader,
		"main",
		nullptr
	);

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

	// 先创建一个空的Pipeline Layout
	VkPipelineLayoutCreateInfo pipelineLayout;
	pipelineLayout.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayout.pNext = nullptr;
	pipelineLayout.flags = 0;
	pipelineLayout.setLayoutCount = 0; // Optional
	pipelineLayout.pSetLayouts = nullptr; // Optional
	pipelineLayout.pushConstantRangeCount = 0; // Optional
	pipelineLayout.pPushConstantRanges = nullptr; // Optional

	if (VK_SUCCESS != vkCreatePipelineLayout(vkDevice, &pipelineLayout, nullptr, &graphicsPipelineLayout))
	{
		throw std::runtime_error("Failed to create Graphics Pipeline");
	}

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
	pipelineCreateInfo.layout = graphicsPipelineLayout; //先渲染一个三角形出来吧……
	pipelineCreateInfo.renderPass = renderPass;
	pipelineCreateInfo.subpass = 0;
	pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
	pipelineCreateInfo.basePipelineIndex = -1;

	if (VK_SUCCESS != vkCreateGraphicsPipelines(vkDevice, VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &graphicsPipeline))
	{
		throw std::runtime_error("Failed to create Graphics Pipeline");
	}
}

uint32_t VkRBInterface::FindMemoryType(const VkMemoryRequirements& memoryRequirements, VkMemoryPropertyFlags requiredFlags, VkMemoryPropertyFlags preferredFlags)
{
	VkPhysicalDeviceMemoryProperties memoryProperties;
	vkGetPhysicalDeviceMemoryProperties(vkPhysicalDevice, &memoryProperties);

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