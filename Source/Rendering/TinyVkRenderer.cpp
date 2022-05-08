/*
    Module Description:
    The implementation of TinyVkRenderer.

    Author:
    Zhao Han Ning

    Created Date:
    2022.4.18

    Notes:
    日了,Module 和 Macro 似乎并不能很好地混用？那么就得思考Conditonal Compilation如何在Module中使用
*/

#define WINDOW_APP
#define VK_USE_PLATFORM_WIN32_KHR
#define STB_IMAGE_IMPLEMENTATION

#define RENDER_DOC_ENABLE

module;
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <Utilities\stb\stb_image.h>
#include <vulkan\vulkan.h>
module TinyVkRenderer;

import RenderDocPlugin;

TinyVkRenderer::TinyVkRenderer(uint32_t windowWidth, uint32_t windowHeight)
    : window(windowWidth, windowHeight), windowWidth(windowWidth), windowHeight(windowHeight)
{
#ifdef RENDER_DOC_ENABLE
    //RenderDocWindowsInit((void*)&vkInstance, (void*)&window.GetHWDN());
#endif

    InitVulkanInstance();
    InitVulkanPhysicalDevices();
    InitVulkanLogicalDevice();

    InitCommandPool();
    InitCommandBuffers();

    //-------//
    //InvertImageInit();
    PresentImageInit();
    //-------//
}

TinyVkRenderer::~TinyVkRenderer()
{
    PresentImageClean();
    //InvertImageResourceClean();
    vkDestroyCommandPool(vkDevice, vkCommandPool, nullptr);
    vkDestroyDevice(vkDevice, nullptr);
    vkDestroyInstance(vkInstance, nullptr);
}

void TinyVkRenderer::Run()
{
    while (window.Update())
    {
        //StartRenderDocCapture();
        //TriggerRenderDocCapture();
        Update();
        Render();
        //EndRenderDocCapture();
    }
    vkDeviceWaitIdle(vkDevice);
}

void TinyVkRenderer::Update()
{
}

void TinyVkRenderer::Render()
{
    PreRender();

    //InvertImageRender();
    PresentImageRender();

    PostRender();
}

void TinyVkRenderer::InitVulkanInstance()
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

void TinyVkRenderer::EnableInstanceLayers(std::vector<const char*>& enabledInstanceLayers)
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

void TinyVkRenderer::EnableInstanceExtensions(std::vector<const char*>& enabledInstanceExtension)
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

void TinyVkRenderer::InitVulkanPhysicalDevices()
{
    uint32_t numPhysicalDevices = 0;
    
    if (VK_SUCCESS != vkEnumeratePhysicalDevices(vkInstance, &numPhysicalDevices, nullptr))
    {
        throw std::runtime_error("Failed to enumerate physical devices");
    }

    vkPhysicalDevices.resize(numPhysicalDevices);
    vkEnumeratePhysicalDevices(vkInstance, &numPhysicalDevices, vkPhysicalDevices.data());

    // TODO: 
    // 1. Make a function to choose the best device if there's any.
    // 2. Find a way to save the physical devices info below,
    //    we'll need a way to determine some run-time behaviors
    //    based on these info.

    // Get physical device properties.
    VkPhysicalDeviceProperties properties;
    vkGetPhysicalDeviceProperties(vkPhysicalDevices[0], &properties);

    // Get physical device features.
    VkPhysicalDeviceFeatures features;
    vkGetPhysicalDeviceFeatures(vkPhysicalDevices[0], &features);

    // Get physical device memory properties.
    VkPhysicalDeviceMemoryProperties memoryProperties;
    vkGetPhysicalDeviceMemoryProperties(vkPhysicalDevices[0], &memoryProperties);

    // Get queue family properties of the physical device
    uint32_t queueFamilyCount;
    vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevices[0], &queueFamilyCount, nullptr);
    
    std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevices[0], &queueFamilyCount, queueFamilyProperties.data());

    // Do something to choose the appropriate Queue Family
    // For now I'll just set it to default 0 which refers to 15: graphics/compute/transfer/sparse bits are enabled.
    currentQueueFamilyIndex = 0;
}

void TinyVkRenderer::InitWindowSurface()
{
#if defined(WINDOW_APP) && defined(VK_USE_PLATFORM_WIN32_KHR)
    VkWin32SurfaceCreateInfoKHR createInfo;
    createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.hinstance = window.GetInstance();
    createInfo.hwnd = window.GetHWDN();

    if (VK_SUCCESS != vkCreateWin32SurfaceKHR(vkInstance, &createInfo, nullptr, &vkSurface))
    {
        throw std::runtime_error("Failed to create Win32 Surface");
    }

    // Check if the queue is able to present images
	VkBool32 supported;
	if (VK_SUCCESS != vkGetPhysicalDeviceSurfaceSupportKHR(vkPhysicalDevices[0], 0, vkSurface, &supported))
	{
		throw std::runtime_error("Failed to get surface presentation support");
	}
	if (!supported)
	{
		throw std::runtime_error("Current selected queue doesn't support Presentation");
	}
#endif
}

void TinyVkRenderer::InitVulkanLogicalDevice()
{
    // 太多Hardcoded了……未来慢慢丰富吧
    // Queue Family Index默认是0
    // Physical Device默认是0（目前我电脑也只有一个……也就是6900xt）
    std::vector<const char*> enabledDeviceLayers;
    EnableDeviceLayers(enabledDeviceLayers);

    std::vector<const char*> enabledDeviceExtensions;
    EnableDeviceExtensions(enabledDeviceExtensions);

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfo(1);
    queueCreateInfo[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo[0].pNext = nullptr;
    queueCreateInfo[0].flags = 0;
    queueCreateInfo[0].queueFamilyIndex = currentQueueFamilyIndex;
    queueCreateInfo[0].queueCount = 1;
    float priorities[1] = { 0 };
    queueCreateInfo[0].pQueuePriorities = priorities;

    VkDeviceCreateInfo createInfo;
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    // TODO: Queue的动态选择和创建
    createInfo.pQueueCreateInfos = queueCreateInfo.data();
    createInfo.queueCreateInfoCount = 1;
    // TODO: layer和extension的动态加载
    createInfo.enabledLayerCount = static_cast<uint32_t>(enabledDeviceLayers.size());
    createInfo.ppEnabledLayerNames = enabledDeviceLayers.data();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(enabledDeviceExtensions.size());
    createInfo.ppEnabledExtensionNames = enabledDeviceExtensions.data();
    // TODO: 可以把Physical Device创建的feature直接拿来用,！但会增加性能开销！
    // 一个更加自然的实现就是用到什么就enable什么feature,但这个就比较复杂了……
    createInfo.pEnabledFeatures = nullptr;

    if (VK_SUCCESS != vkCreateDevice(vkPhysicalDevices[0], &createInfo, nullptr, &vkDevice))
    {
        throw std::runtime_error("Failed to create logical device");
    }

    vkQueues.resize(1);
    vkGetDeviceQueue(vkDevice, 0, 0, &vkQueues[0]);
    if (vkQueues[0] == VK_NULL_HANDLE)
    {
        throw std::runtime_error("Failed to get queue");
    }
}

void TinyVkRenderer::EnableDeviceLayers(std::vector<const char*>& enabledDeviceLayers)
{
    uint32_t layerCount;
    std::vector<VkLayerProperties> layers;

    vkEnumerateDeviceLayerProperties(vkPhysicalDevices[0], &layerCount, nullptr);
    if (layerCount != 0)
    {
        layers.resize(layerCount);
        vkEnumerateDeviceLayerProperties(vkPhysicalDevices[0], &layerCount, layers.data());
    }
}

void TinyVkRenderer::EnableDeviceExtensions(std::vector<const char*>& enabledDeviceExtensions)
{
#ifdef WINDOW_APP
    enabledDeviceExtensions.push_back("VK_KHR_swapchain");
#endif

    uint32_t extensionCount;
    std::vector<VkExtensionProperties> extensions;

    vkEnumerateDeviceExtensionProperties(vkPhysicalDevices[0], nullptr, &extensionCount, nullptr);
    if (extensionCount != 0)
    {
        extensions.resize(extensionCount);
        vkEnumerateDeviceExtensionProperties(vkPhysicalDevices[0], nullptr, &extensionCount, extensions.data());
    }
}

void TinyVkRenderer::InitSwapChain()
{
    //TODO: 需要从外面传很多配置参数进来……然后还要做各种检查……啊……
    VkSurfaceCapabilitiesKHR surfaceCap;
    if (VK_SUCCESS != vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vkPhysicalDevices[0], vkSurface, &surfaceCap))
    {
        throw std::runtime_error("Failed to get surface capabilities of physical device");
    }

    uint32_t supportedFormatCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(vkPhysicalDevices[0], vkSurface, &supportedFormatCount, nullptr);
    std::vector<VkSurfaceFormatKHR> surfaceFormats(supportedFormatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(vkPhysicalDevices[0], vkSurface, &supportedFormatCount, surfaceFormats.data());
    // CheckFormat(requiredFormat, availableFormat) 总之要选一个可用的format出来

    // 该检查要放在选用 queue family 并且需要 window 的时候
    //VkBool32 supported = VK_FALSE;
    //if (VK_SUCCESS != vkGetPhysicalDeviceSurfaceSupportKHR(vkPhysicalDevices[0], 0, vkSurface, &supported))
    //{
    //    throw std::runtime_error("Something went wrong when checking if queue family supports presesentation to surface");
    //}

    swapChainImageFormat = VK_FORMAT_R8G8B8A8_UNORM;
    swapChainExtent = surfaceCap.currentExtent;

    // Swap Chain算是相当重型了……如下好多参数都决定了整体的基调,需要多加重视
    VkSwapchainCreateInfoKHR createInfo;
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.surface = vkSurface;
    createInfo.minImageCount = 3; //at least 3
    createInfo.imageFormat = swapChainImageFormat;
    createInfo.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR; // 现在有一大堆 Color Space...
    createInfo.imageExtent = swapChainExtent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.queueFamilyIndexCount = 0;  // ignored if using exclusive mode
    createInfo.pQueueFamilyIndices = nullptr; // ignored if using exclusive mode
    createInfo.preTransform = surfaceCap.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
    createInfo.clipped = VK_TRUE; // Don't render into the invisible region
    createInfo.oldSwapchain = vkSwapChain;

    if (VK_SUCCESS != vkCreateSwapchainKHR(vkDevice, &createInfo, nullptr, &vkSwapChain))
    {
        throw std::runtime_error("Failed to create Swap Chain");
    }

    uint32_t swapChainImageCount = 0;
    vkGetSwapchainImagesKHR(vkDevice, vkSwapChain, &swapChainImageCount, nullptr);
    vkSwapChainImages.resize(swapChainImageCount);
    vkGetSwapchainImagesKHR(vkDevice, vkSwapChain, &swapChainImageCount, vkSwapChainImages.data());
}

uint32_t TinyVkRenderer::GetAvailableImage(uint64_t waitTimeNano)
{
    // TODO: 记得给这个参数,后续还要把Semaphore和Fence作为参数传进来
    // 或许不应该result image Index 不然switch case 里面不好 return
    uint32_t imageIndex = 0;
    VkResult result = vkAcquireNextImageKHR(vkDevice, vkSwapChain, waitTimeNano, VK_NULL_HANDLE, VK_NULL_HANDLE, &imageIndex);
    switch (result)
    {
    case VK_SUCCESS:
    {
        return imageIndex;
    }
    case VK_NOT_READY:
    {
        // do something
    }break;
    default:
    {
        // do what?
    }
    };

    return imageIndex;
}

void TinyVkRenderer::InitCommandPool()
{
    VkCommandPoolCreateInfo createInfo;
    createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    createInfo.queueFamilyIndex = currentQueueFamilyIndex;

    if (VK_SUCCESS != vkCreateCommandPool(vkDevice, &createInfo, nullptr, &vkCommandPool))
    {
        throw std::runtime_error("Failed to create command pool");
    }
}

void TinyVkRenderer::InitCommandBuffers()
{
    VkCommandBufferAllocateInfo allocInfo;
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.pNext = nullptr;
    allocInfo.commandPool = vkCommandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;

    if (VK_SUCCESS != vkAllocateCommandBuffers(vkDevice, &allocInfo, &cmd))
    {
        throw std::runtime_error("Failed to create command buffer");
    }
    // There is no need to explicitly free all command buffers
    // Destroy the Command Pool will also free all cmd buffers as well as associated resources.
}

void TinyVkRenderer::PreRender()
{
}

void TinyVkRenderer::PostRender()
{
    // After this point, all works in command buffers are finished.
    vkQueueWaitIdle(vkQueues[0]);
}

void TinyVkRenderer::BeginCommandBuffer()
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

void TinyVkRenderer::EndCommandBuffer()
{
    if (VK_SUCCESS != vkEndCommandBuffer(cmd))
    {
        throw std::runtime_error("Failed to end Buffer");
    }
}

void TinyVkRenderer::SubmitCommandBuffer()
{
    VkSubmitInfo submitInfo;
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.pNext = nullptr;
    submitInfo.waitSemaphoreCount = 0;
    submitInfo.pWaitSemaphores = nullptr;
    submitInfo.pWaitDstStageMask = nullptr;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &cmd;
    submitInfo.signalSemaphoreCount = 0;
    submitInfo.pSignalSemaphores = nullptr;

    if (VK_SUCCESS != vkQueueSubmit(vkQueues[0], 1, &submitInfo, VK_NULL_HANDLE))
    {
        throw std::runtime_error("Failed to submit queue");
    }
}

void TinyVkRenderer::Temp()
{
    // Resource State
    // Vulkan是真的复杂啊……录制cmd,执行就是分开的,大量并行的操作,pipeline stage之间如何的转换……
    // 所有的这些变化,都是程序员负责完善它们,把一个一个的资源做好同步,避免hazard的出现,唉……
    // 现在只是学习,还不算复杂,等到真正开始使用了,完全可以预料到一团乱麻的情况,所以Frame Graph的出现就是基于这种需求

    // Barrier的作用：
    // 1. 为内存访问提供同步机制
    // 2. 改变Resource State
    // vkCmdPipelineBarrier()
    // VkMemoryBarrier globalBarriers;      //粒度最大,在memory层确保所有SrcStage的工作完成,才会到DstStage去
    // VkBufferMemoryBarrier bufferBarriers;//只针对Buffer,所以粒度很细
    // VkImageMemoryBarrier imageBarriers;  //只针对Image

    // Buffer 数据操作
    // vkCmdFillBuffer()  //用某个值初始化Buffer,在丛哥手下做事时遇到的……Offset和Size都是4个bytes为单位（DX12也是）,metal居然只有1个byte,粒度是真的细
    // vkCmdUpdateBuffer() //用于更新小部分的值  2^16 65536 bytes的上限,适用于Shader会访问的Uniform buffer和shader storage block
    // vkCmdCopyBuffer() //用于很大一块数据的转移

    // Images 数据操作
    // vkCmdClearColorImage(); //只接受GENERAL和TRANSFER_DST_OPTIMAL的Layout执行该操作,我人傻了,还得Barrier改变Layout才能用……ASPECT_COLOR_BIT
    // vkCmdClearDepthStencilImage(); //layout同上,clear value比较特殊,STENCIL & DEPTH BIT
    // *重点*
    // vkCmdCopyBufferToImage()
    // vkCmdCopyImageToBuffer() 
    // vkCmdCopyImage()
    // vkCmdBlitImage() // Blit is short for block image transfer and refers to the operation of not only copying image data, but potentially also processing it along the way.
    // Blit 这个操作非常灵活 图片可以垂直/水平翻转或旋转180°,如果大小不一样,会自动用点/双线性采样Filter,十分牛逼。
    // *重点*
}

void TinyVkRenderer::CheckFormatProperties()
{
    // Image 有两种 Tiling Mode：Linear 和 Optimal
    // Linear就是从左上到右下的标准线性数据排列
    // 下面查的就是各种Format分别对以上两种情况的支持程度
    // 同时还有对 Formatted Buffer 的支持程度
    // 该查询方法是比较泛泛的
    VkFormatProperties properties;
    vkGetPhysicalDeviceFormatProperties(vkPhysicalDevices[0], /*VkFomrat*/swapChainImageFormat, &properties);

    // 专门针对Image的Format Properties检查要调用：
    VkImageFormatProperties imageProperties;
    vkGetPhysicalDeviceImageFormatProperties(
        vkPhysicalDevices[0],
        /*VkFomrat*/VK_FORMAT_R8_UNORM,
        VK_IMAGE_TYPE_2D,
        VK_IMAGE_TILING_LINEAR,
        VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
        0,
        &imageProperties
    );
}

void TinyVkRenderer::AllocateMemory(uint32_t memoryType)
{

}

uint32_t TinyVkRenderer::FindMemoryType(const VkMemoryRequirements& memoryRequirements, VkMemoryPropertyFlags requiredFlags, VkMemoryPropertyFlags preferredFlags)
{
	VkPhysicalDeviceMemoryProperties memoryProperties;
	vkGetPhysicalDeviceMemoryProperties(vkPhysicalDevices[0], &memoryProperties);

    uint32_t selectedType = ~0u;
    uint32_t memoryType;

    for (memoryType = 0; memoryType < 32; memoryType++){
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

void TinyVkRenderer::CreateDescriptorSetLayout()
{
    std::vector<VkDescriptorSetLayoutBinding> bindings;

    bindings.emplace_back(
        0,
        VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        1,
        VK_SHADER_STAGE_COMPUTE_BIT,
        nullptr
    );

    bindings.emplace_back(
        1,
        VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
        1,
        VK_SHADER_STAGE_COMPUTE_BIT,
        nullptr
    );

    VkDescriptorSetLayoutCreateInfo createInfo;
    createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.bindingCount = 2;
    createInfo.pBindings = bindings.data();

	if (VK_SUCCESS != vkCreateDescriptorSetLayout(vkDevice, &createInfo, nullptr, &descriptorSetLayout))
	{
		throw std::runtime_error("Failed to create descriptor set layout");
	}
}

void TinyVkRenderer::CreateDescriptorPool()
{
    std::vector<VkDescriptorPoolSize> poolContent;
    poolContent.emplace_back(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1);
	poolContent.emplace_back(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1);

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

void TinyVkRenderer::AllocateDescriptorSets()
{
    size_t descriptorSetCount = 1;

    VkDescriptorSetAllocateInfo allocInfo;
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.pNext = nullptr;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = descriptorSetCount;
    allocInfo.pSetLayouts = &descriptorSetLayout;

    descriptorSet.resize(descriptorSetCount);

	if (VK_SUCCESS != vkAllocateDescriptorSets(vkDevice, &allocInfo, &descriptorSet[0]))
	{
		throw std::runtime_error("Failed to create descriptor set");
	}
}

void TinyVkRenderer::UpdateDescriptorSets()
{
    VkDescriptorImageInfo srcImageInfo = { srcSampler , srcImageView, VK_IMAGE_LAYOUT_GENERAL };
    std::vector<VkWriteDescriptorSet> writeDescriptors;
    VkWriteDescriptorSet writeDescriptor;
    writeDescriptor.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeDescriptor.pNext = 0;
    writeDescriptor.dstSet = descriptorSet[0];
    writeDescriptor.dstBinding = 0;
    writeDescriptor.dstArrayElement = 0; // Starting element index in that array
    writeDescriptor.descriptorCount = 1; // Number of descriptors to update
    writeDescriptor.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER; // Determine which one of the following *ARRAY* of data will be chosen
    // Try Union? 
    writeDescriptor.pImageInfo = &srcImageInfo;
    writeDescriptors.push_back(std::move(writeDescriptor));

    VkDescriptorImageInfo dstImageInfo = { VK_NULL_HANDLE , dstImageView , VK_IMAGE_LAYOUT_GENERAL}; //没找到存储用什么Layout?
    writeDescriptors.emplace_back(VkWriteDescriptorSet{
        VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        0,
        descriptorSet[0],
        1,
        0,
        1,
        VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
        &dstImageInfo
    });

    //VkDescriptorImageInfo imageInfo = { sampler, ImageView, ImageLayout };
    // Uniform buffer has some extra limits, check device properties.
    //VkDescriptorBufferInfo bufferInfo = { buffer, offset, range(or size?)};

    //VkCopyDescriptorSet copyDescriptor;
    //copyDescriptor.sType = VK_STRUCTURE_TYPE_COPY_DESCRIPTOR_SET;
    //copyDescriptor.pNext = nullptr;
    //// srcSet and dstSet can be the same, as long as the copied descriptor don't overlap
    //copyDescriptor.srcSet = ? ;
    //copyDescriptor.srcBinding = ? ;
    //copyDescriptor.srcArrayElement = ? ;
    //copyDescriptor.dstSet = ? ;
    //copyDescriptor.dstBinding = ? ;
    //copyDescriptor.dstArrayElement = ? ;
    //copyDescriptor.descriptorCount = ? ; // Number of descriptors to update
        
    // Remember the synchronization! Device CANNOT access the referenced descriptor sets at this point
    // 思考：更新的最佳时机在什么时候？反正不可能是cmd正在执行的途中
    vkUpdateDescriptorSets(vkDevice, writeDescriptors.size(), writeDescriptors.data(), 0, nullptr);
    // Update之后,即可调用 vkCmdBindDescriptorSets()
}

void TinyVkRenderer::CreateShaderModule(const std::string& shaderFile, VkShaderModule& mod)
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

    if (VK_SUCCESS != vkCreateShaderModule(vkDevice, &createInfo ,nullptr ,&mod))
    {
        throw std::runtime_error("Failed to create shader module");
    }
}

void TinyVkRenderer::CreateComputePipeline()
{
    // Test Specialization Constant...
    std::vector<uint32_t> localWorkGroup = { 1, 1, 1 };

    std::vector<VkSpecializationMapEntry> entries;
    for (size_t i = 0; i < localWorkGroup.size(); i++)
    {
        entries.emplace_back(i, i * sizeof(uint32_t), sizeof(uint32_t));
    }

    VkSpecializationInfo specInfo;
    specInfo.mapEntryCount = localWorkGroup.size();
    specInfo.pMapEntries = entries.data();
    specInfo.dataSize = sizeof(uint32_t) * localWorkGroup.size();
    specInfo.pData = localWorkGroup.data();
    // Test Specialization Constant...

    // Shader Module
    VkPipelineShaderStageCreateInfo stageCreateInfo;
    stageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    stageCreateInfo.pNext = nullptr;
    stageCreateInfo.flags = 0;
    stageCreateInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    stageCreateInfo.module = shaderModule;
    stageCreateInfo.pName = "main";
    stageCreateInfo.pSpecializationInfo = &specInfo;

    VkComputePipelineCreateInfo createInfo;
    createInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.stage = stageCreateInfo;
    createInfo.layout = pipelineLayout;
    // Pipeline Derivatives...啥玩意儿？？
    createInfo.basePipelineHandle = VK_NULL_HANDLE;
    createInfo.basePipelineIndex = -1;
    
    //pipelineCache 的作用仅仅只是加速Pipeline的创建过程吗？不能省下一些诸如CreateInfo的创建？
    if (VK_SUCCESS != vkCreateComputePipelines(vkDevice, VK_NULL_HANDLE, 1, &createInfo, nullptr, &computePipeline))
    {
        throw std::runtime_error("Failed to create compute pipeline");
    }
}

void TinyVkRenderer::CreatePipelineLayout()
{
    VkPipelineLayoutCreateInfo createInfo;
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.setLayoutCount = 1;
    createInfo.pSetLayouts = &descriptorSetLayout;
    createInfo.pushConstantRangeCount = 0;
    createInfo.pPushConstantRanges = nullptr;

	if (VK_SUCCESS != vkCreatePipelineLayout(vkDevice, &createInfo, nullptr, &pipelineLayout))
	{
		throw std::runtime_error("Failed to create pipeline layout");
	}
}

void TinyVkRenderer::CreatePipelineCache()
{
    // 可以试一下创建Pipeline时用上Cache,看是否会有加速？
    VkPipelineCacheCreateInfo createInfo;
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.initialDataSize = 0;
    createInfo.pInitialData = nullptr;

	if (VK_SUCCESS != vkCreatePipelineCache(vkDevice, &createInfo, nullptr, &pipelineCache))
	{
		throw std::runtime_error("Failed to create pipeline cache");
	}
}

void TinyVkRenderer::InvertImageInit()
{
    CreateSrcSampler();
    CreateSrcImage("../Resources/worley.jpg");
    CreateDstImage();

    CreateSrcImageView();
    CreateDstImageView();

    // Create Shader
    CreateShaderModule("../Rendering/Shaders/Image.spv", shaderModule);
    // Create Descriptor Set Layout
    CreateDescriptorSetLayout();
	// Create Pipeline Layout
	CreatePipelineLayout();
    // Create Pipeline
	CreateComputePipeline();
    // Create Descriptor Pool
    CreateDescriptorPool();
    // Allocate Descriptor Set
    AllocateDescriptorSets();
    // Bind Resources to Descriptor Set
    UpdateDescriptorSets();
}

void TinyVkRenderer::InvertImageResourceClean()
{
	vkDestroyDescriptorPool(vkDevice, descriptorPool, nullptr);
	vkDestroyPipeline(vkDevice, computePipeline, nullptr);
	vkDestroyPipelineLayout(vkDevice, pipelineLayout, nullptr);
	vkDestroyDescriptorSetLayout(vkDevice, descriptorSetLayout, nullptr);
    vkDestroyShaderModule(vkDevice, shaderModule, nullptr);
    
	vkDestroyImageView(vkDevice, dstImageView, nullptr);
	vkDestroyImageView(vkDevice, srcImageView, nullptr);

	vkDestroyImage(vkDevice, dstImage, nullptr);
	vkDestroyImage(vkDevice, srcImage, nullptr);

	vkFreeMemory(vkDevice, dstMemory, nullptr);
	vkFreeMemory(vkDevice, srcMemory, nullptr);

    vkDestroySampler(vkDevice, srcSampler, nullptr);
}

void TinyVkRenderer::InvertImageRender()
{
	BeginCommandBuffer();

	// 计算管线和图形管线的Binding可能需要解耦,是一个相当复杂的过程啊唉……
    // 妈的 光追混合渲染管线咋弄？？？
    // Bind pipeline and command buffer
	vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, computePipeline);
	vkCmdBindDescriptorSets(
		cmd, VK_PIPELINE_BIND_POINT_COMPUTE, pipelineLayout,
		0, descriptorSet.size(), descriptorSet.data(), // Descriptor
		0, nullptr); // Dynamic uniform or shader storage bindings offset.
	// TODO 找机会试一下Indirect Dispatch
	vkCmdDispatch(cmd, 16, 16, 1);

	EndCommandBuffer();
	SubmitCommandBuffer();
}

void TinyVkRenderer::InvertImageUpdate()
{
	// Update Image Resource
	UpdateDescriptorSets();
}

void TinyVkRenderer::CreateSrcSampler()
{
    VkSamplerCreateInfo createInfo;
    createInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.magFilter = VK_FILTER_LINEAR;
    createInfo.minFilter = VK_FILTER_LINEAR;
    createInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    createInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    createInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    createInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    createInfo.mipLodBias = 0;
    createInfo.anisotropyEnable = VK_FALSE;
    //createInfo.maxAnisotropy = ;
    createInfo.compareEnable = VK_FALSE;
    //createInfo.compareOp = ;
    createInfo.minLod = 0;
    createInfo.maxLod = 1;
    createInfo.borderColor = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
    createInfo.unnormalizedCoordinates = VK_FALSE;

	if (VK_SUCCESS != vkCreateSampler(vkDevice, &createInfo, nullptr, &srcSampler))
	{
		throw std::runtime_error("Failed to create src sampler");
	}
}

void TinyVkRenderer::CreateSrcImage(const std::string& filename)
{
	// Load image
	int width, height, channels;
	unsigned char* data = stbi_load(filename.data(), &width, &height, &channels, STBI_rgb_alpha);
	// 试一下用一个Memory Object放两张图片，下面size可以用来作为第二张图的offset
	// VkDeviceSize imageSize = static_cast<VkDeviceSize>(width * height * 3); // 24 bit

	// Create Image Object
	VkImageCreateInfo createInfo;
	createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.flags = 0;
	createInfo.imageType = VK_IMAGE_TYPE_2D;
	createInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
	createInfo.extent = { uint32_t(width), uint32_t(height), 1 };
	createInfo.arrayLayers = 1;
	createInfo.mipLevels = 1;
	createInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	createInfo.tiling = VK_IMAGE_TILING_LINEAR; // 如果CPU要改动这个Image,那就设置成Linear,不然就用Optimal
	createInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	createInfo.queueFamilyIndexCount = 0;
	createInfo.pQueueFamilyIndices = nullptr;
	createInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

	if (VK_SUCCESS != vkCreateImage(vkDevice, &createInfo, nullptr, &srcImage))
	{
		throw std::runtime_error("Failed to create src Image");
	}

	// Check Requirement
	VkMemoryRequirements memoryRequirements;
	vkGetImageMemoryRequirements(vkDevice, srcImage, &memoryRequirements);

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

	if (VK_SUCCESS != vkAllocateMemory(vkDevice, &allocInfo, nullptr, &srcMemory))
	{
		throw std::runtime_error("Failed to create device memory");
	}

	{ // Feed Image data into memory
		void* mappedData;
		if (VK_SUCCESS != vkMapMemory(vkDevice, srcMemory, 0, VK_WHOLE_SIZE, 0, &mappedData))
		{
			throw std::runtime_error("Failed to Map Memory");
		}

        memcpy(mappedData, data, width * height * 32);

		// Flush the memory so the device can actually use the image data.
		VkMappedMemoryRange range;
		range.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
		range.pNext = nullptr;
		range.memory = srcMemory;
		range.offset = 0;
		range.size = VK_WHOLE_SIZE;
		vkFlushMappedMemoryRanges(vkDevice, 1, &range);

		vkUnmapMemory(vkDevice, srcMemory);
	}
	stbi_image_free(data);

	// Bind memory object and image object
	if (VK_SUCCESS != vkBindImageMemory(vkDevice, srcImage, srcMemory, 0))
	{
		throw std::runtime_error("Failed to bind image and memory");
	}
}

void TinyVkRenderer::CreateDstImage()
{
	// Create Image Object
	VkImageCreateInfo createInfo;
	createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.flags = 0;
	createInfo.imageType = VK_IMAGE_TYPE_2D;
	createInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
	createInfo.extent = { 256, 256, 1 };
	createInfo.arrayLayers = 1;
	createInfo.mipLevels = 1;
	createInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	createInfo.tiling = VK_IMAGE_TILING_OPTIMAL; // 如果CPU要改动这个Image,那就设置成Linear,不然就用Optimal
	createInfo.usage = VK_IMAGE_USAGE_STORAGE_BIT;
	createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	createInfo.queueFamilyIndexCount = 0;
	createInfo.pQueueFamilyIndices = nullptr;
	createInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

	if (VK_SUCCESS != vkCreateImage(vkDevice, &createInfo, nullptr, &dstImage))
	{
		throw std::runtime_error("Failed to create dst Image");
	}

	// Check Requirement
	VkMemoryRequirements memoryRequirements;
	vkGetImageMemoryRequirements(vkDevice, dstImage, &memoryRequirements);

	// Find Suitable Memory Type
	uint32_t memoryIndex = FindMemoryType(
		memoryRequirements,
		0,
		0
	);

	// Allocate Memory
	VkMemoryAllocateInfo allocInfo;
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.pNext = nullptr;
	allocInfo.allocationSize = 1024 * 1024 * sizeof(float);
	allocInfo.memoryTypeIndex = memoryIndex;

	if (VK_SUCCESS != vkAllocateMemory(vkDevice, &allocInfo, nullptr, &dstMemory))
	{
		throw std::runtime_error("Failed to create device memory");
	}

	// Bind memory object and image object
	if (VK_SUCCESS != vkBindImageMemory(vkDevice, dstImage, dstMemory, 0))
	{
		throw std::runtime_error("Failed to bind image and memory");
	}
}

void TinyVkRenderer::CreateSrcImageView()
{
	VkImageViewCreateInfo createInfo;
	createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.flags = 0;
	createInfo.image = srcImage;
	createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	createInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
	createInfo.components = VkComponentMapping{};
	createInfo.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };

	if (VK_SUCCESS != vkCreateImageView(vkDevice, &createInfo, nullptr, &srcImageView))
	{
		throw std::runtime_error("Failed to create Image view");
	}
}

void TinyVkRenderer::CreateDstImageView()
{
	VkImageView imageView;
	VkImageViewCreateInfo createInfo;
	createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.flags = 0;
	createInfo.image = dstImage;
	createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	createInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
    createInfo.components = VkComponentMapping{};
	createInfo.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };

	if (VK_SUCCESS != vkCreateImageView(vkDevice, &createInfo, nullptr, &dstImageView))
	{
		throw std::runtime_error("Failed to create Image view");
	}
}

void TinyVkRenderer::PresentImageInit()
{
    InitWindowSurface();
    InitSwapChain();

    CreateVertexBuffer();
    CreatePresentImageView();
    CreateRenderPass();
    CreateFramebuffer();
    CreateGraphicsPipeline();
}

void TinyVkRenderer::PresentImageClean()
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
	for (size_t i = 0; i < presentImageViews.size(); i++)
	{
		vkDestroyImageView(vkDevice, presentImageViews[i], nullptr);
	}
    vkDestroySwapchainKHR(vkDevice, vkSwapChain, nullptr);
}

void TinyVkRenderer::PresentImageRender()
{
    uint32_t imageIndex = GetAvailableImage(UINT64_MAX);
    //TODO: Present Loop
    BeginCommandBuffer();
    CmdBeginRenderPass(framebuffers[imageIndex]);
    CmdDraw();
    CmdEndRenderPass();
	EndCommandBuffer();
	SubmitCommandBuffer();
    // After submission, the rendering works are done. Now the image is ready for presentation.
    ImagePresentation(imageIndex);
}

void TinyVkRenderer::CreateVertexBuffer()
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

	if (VK_SUCCESS != vkAllocateMemory(vkDevice, &allocInfo, nullptr, &srcMemory))
	{
		throw std::runtime_error("Failed to create device memory");
	}

	{ // Mapping
		void* mappedData;
		if (VK_SUCCESS != vkMapMemory(vkDevice, srcMemory, 0, VK_WHOLE_SIZE, 0, &mappedData))
		{
			throw std::runtime_error("Failed to Map Memory");
		}

        memcpy(mappedData, vertexData.data(), sizeof(Vertex) * 3);

		// Flush the memory so the device can actually use the image data.
		VkMappedMemoryRange range;
		range.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
		range.pNext = nullptr;
		range.memory = srcMemory;
		range.offset = 0;
		range.size = VK_WHOLE_SIZE;
		vkFlushMappedMemoryRanges(vkDevice, 1, &range);

		vkUnmapMemory(vkDevice, srcMemory);
	}

	// Bind memory object and image object
	if (VK_SUCCESS != vkBindBufferMemory(vkDevice, vertexBuffer, srcMemory, 0))
	{
		throw std::runtime_error("Failed to bind buffer and memory");
	}
}

void TinyVkRenderer::CreatePresentImageView()
{
    presentImageViews.resize(vkSwapChainImages.size());
    for (size_t i = 0; i < vkSwapChainImages.size(); i++)
    {
		VkImageViewCreateInfo createInfo;
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.pNext = nullptr;
		createInfo.flags = 0;
		createInfo.image = vkSwapChainImages[i];
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = swapChainImageFormat;
		createInfo.components = VkComponentMapping{};
		createInfo.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };

		if (VK_SUCCESS != vkCreateImageView(vkDevice, &createInfo, nullptr, &presentImageViews[i]))
		{
			throw std::runtime_error("Failed to create present image view");
		}
    }
}

void TinyVkRenderer::CreateRenderPass()
{
    VkAttachmentDescription attachmentDes;
	attachmentDes.flags = 0; // for memory aliasing
	attachmentDes.format = swapChainImageFormat;
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
	VkSubpassDependency subpassDen;
	subpassDen.srcSubpass = VK_SUBPASS_EXTERNAL;
	subpassDen.dstSubpass = 0;
	subpassDen.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	subpassDen.dstStageMask = 0;
	subpassDen.srcAccessMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	subpassDen.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	//subpassDen.dependencyFlags = ?;

    VkRenderPassCreateInfo createInfo;
    createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.attachmentCount = 1;
    createInfo.pAttachments = &attachmentDes;
    createInfo.subpassCount = 1;
	createInfo.pSubpasses = &subpassDes;
	createInfo.dependencyCount = 1;
	createInfo.pDependencies = &subpassDen;

	if (VK_SUCCESS != vkCreateRenderPass(vkDevice, &createInfo, nullptr, &renderPass))
	{
		throw std::runtime_error("Failed to create Render Pass");
	}
}

void TinyVkRenderer::CreateFramebuffer()
{
	framebuffers.resize(vkSwapChainImages.size());
    for (size_t i = 0; i < presentImageViews.size(); i++)
    {
        VkFramebufferCreateInfo createInfo;
        createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        createInfo.pNext = nullptr;
        createInfo.flags = 0;
        createInfo.renderPass = renderPass;
        createInfo.attachmentCount = 1;
        // 千万注意，RenderPass里Attachment References就是会按照创建的顺序作为indices访问如下的array
        // 可以说是默认两边顺序要一样！千万不要弄错！！！
        createInfo.pAttachments = &presentImageViews[i];
        createInfo.width = swapChainExtent.width;
        createInfo.height = swapChainExtent.height;
        createInfo.layers = 1;

        if (VK_SUCCESS != vkCreateFramebuffer(vkDevice, &createInfo, nullptr, &framebuffers[i]))
        {
            throw std::runtime_error("Failed to create Framebuffer");
        }
    }
}

void TinyVkRenderer::CreateGraphicsPipeline()
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
	InputCreateInfo.flags= 0;
	InputCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	InputCreateInfo.primitiveRestartEnable = VK_FALSE;

    VkViewport viewport = { 0.f, 0.f, swapChainExtent.width, swapChainExtent.height, 0.f, 1.f };
    VkRect2D scissor = { {0,0} , swapChainExtent };
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

void TinyVkRenderer::CmdBeginRenderPass(VkFramebuffer& framebuffer)
{
    VkRect2D area = { {0,0},swapChainExtent };
    VkClearValue clearColor; 
    clearColor.color = { 1.0f, 0.f, 0.f, 1.f };
    VkRenderPassBeginInfo beginInfo;
	beginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	beginInfo.pNext = nullptr;
    beginInfo.renderPass = renderPass;
    beginInfo.framebuffer = framebuffer;
	beginInfo.renderArea = area;
	beginInfo.clearValueCount = 1;
	beginInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(cmd, &beginInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void TinyVkRenderer::CmdEndRenderPass()
{
    vkCmdEndRenderPass(cmd);
}

void TinyVkRenderer::CmdDraw()
{
    //TODO: Image Layout Transition..
    VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(cmd, 0, 1, &vertexBuffer, offsets);

    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

    vkCmdDraw(cmd, vertexData.size(), 1, 0, 0);

    // subpass dependencies 可以完成renderpass内的 Image Layout Transition
 //   //Image transition for presentation
 //   VkImageMemoryBarrier presentBarrier;
	//presentBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	//presentBarrier.pNext = nullptr;
	//presentBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	//presentBarrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
	//presentBarrier.oldLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL;
	//presentBarrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	//presentBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	//presentBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	//presentBarrier.image = ? ;
	//presentBarrier.subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};
}

void TinyVkRenderer::ImagePresentation(uint32_t imageIndex)
{
    VkResult result;

    VkPresentInfoKHR presentInfo;
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.pNext = nullptr;
	presentInfo.waitSemaphoreCount = 0;
	presentInfo.pWaitSemaphores = nullptr;
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &vkSwapChain;
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
