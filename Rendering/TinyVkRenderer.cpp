/*
    Module Description:
    The implementation of TinyVkRenderer.

    Author:
    Zhao Han Ning

    Created Date:
    2022.4.18

    Notes:
    日了，Module 和 Macro 似乎并不能很好地混用？那么就得思考Conditonal Compilation如何在Module中使用

    一些未来需要实现的点：
    1. Transient 资源类型的使用
    2. 去掉所有hardcoded
    3. 
*/

#define WINDOW_APP
#define VK_USE_PLATFORM_WIN32_KHR

module;
#include <stdexcept>
#include <vulkan\vulkan.h>
module TinyVkRenderer;

import <vector>;


TinyVkRenderer::TinyVkRenderer(uint32_t windowWidth, uint32_t windowHeight)
    : window(windowWidth, windowHeight)
{
    InitVulkanInstance();
    InitVulkanPhysicalDevices();
    InitVulkanLogicalDevice();
    InitSwapChain();
    InitCommandPool();
    InitCommandBuffers();
}

TinyVkRenderer::~TinyVkRenderer()
{
    vkDestroyCommandPool(vkDevice, vkCommandPool, nullptr);
    vkDestroySwapchainKHR(vkDevice, vkSwapChain, nullptr);
    vkDestroyDevice(vkDevice, nullptr);
    vkDestroyInstance(vkInstance, nullptr);
}

void TinyVkRenderer::Run()
{
    while (window.Update())
    {
        Render();
    }
    vkDeviceWaitIdle(vkDevice);
}

void TinyVkRenderer::Render()
{
    // TODO: 一点没写的Render

    // GetAvailableImage();

    // 把渲染命令录进cmd

    // 用 Memory Barrier 做 Image Transition，准备presentation

    // VkPresentInfoKHR presentInfo;
    // vkQueuePresentKHR()
    PreRender();


    PostRender();
}

void TinyVkRenderer::InitVulkanInstance()
{
    // 既然可以在创建Instance之前先获取Instance级别的Layer和Extensions，
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
    createInfo.enabledLayerCount = enabledInstanceLayers.size();
    createInfo.ppEnabledLayerNames = enabledInstanceLayers.data();
    createInfo.enabledExtensionCount = enabledInstanceExtensions.size();
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

    // TODO：获取到该Platform支持的Layers后，要检查是否覆盖了APP所需
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

    // TODO：获取到该Platform支持的Extensions后，要检查是否覆盖了APP所需
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
    
    // Note that we haven't used any of the info above
    // It's safe to assume there will be a pain in the ass
    // to properly sort all these properties out.
    InitWindow();
}

void TinyVkRenderer::InitWindow()
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
    createInfo.enabledLayerCount = enabledDeviceLayers.size();
    createInfo.ppEnabledLayerNames = enabledDeviceLayers.data();
    createInfo.enabledExtensionCount = enabledDeviceExtensions.size();
    createInfo.ppEnabledExtensionNames = enabledDeviceExtensions.data();
    // TODO: 可以把Physical Device创建的feature直接拿来用，！但会增加性能开销！
    // 一个更加自然的实现就是用到什么就enable什么feature，但这个就比较复杂了……
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
        throw std::runtime_error("Failed to get surface capablitities of physical device");
    }

    uint32_t supportedFormatCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(vkPhysicalDevices[0], vkSurface, &supportedFormatCount, nullptr);
    std::vector<VkSurfaceFormatKHR> surfaceFormats(supportedFormatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(vkPhysicalDevices[0], vkSurface, &supportedFormatCount, surfaceFormats.data());
    // CheckFormat(requiredFormat, availableFormat) 总之要选一个可用的format出来

    // 该检查要放在选用queue family并且需要window的时候
    //VkBool32 supported = VK_FALSE;
    //if (VK_SUCCESS != vkGetPhysicalDeviceSurfaceSupportKHR(vkPhysicalDevices[0], 0, vkSurface, &supported))
    //{
    //    throw std::runtime_error("Something went wrong when checking if queue family supports presesentation to surface");
    //}

    // Swap Chain算是相当重型了……如下好多参数都决定了整体的基调，需要多加重视
    VkSwapchainCreateInfoKHR createInfo;
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.surface = vkSurface;
    createInfo.minImageCount = 3; //at least 3
    createInfo.imageFormat = VK_FORMAT_R8G8B8A8_UNORM;
    createInfo.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR; // 现在有一大堆 Color Space...
    createInfo.imageExtent = surfaceCap.currentExtent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.queueFamilyIndexCount = 0;  // ignored if using eclusive mode
    createInfo.pQueueFamilyIndices = nullptr; // ignored if using eclusive mode
    createInfo.preTransform = surfaceCap.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
    createInfo.clipped = VK_TRUE; // Don't render into the invisiable region
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
    // TODO: 记得给这个参数，后续还要把Semaphore和Fence作为参数传进来
    // 或许不应该result image Index 不然switch case里面不好return
    // waitTimeNano = UINT64_MAX;
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
    createInfo.flags = 0;
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
    // There is no need to explicity free all command buffers
    // Destroy the Command Pool will also free all cmd buffers as well as associated resources.
}

void TinyVkRenderer::PreRender()
{
}

void TinyVkRenderer::PostRender()
{
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
