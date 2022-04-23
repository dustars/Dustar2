/*
    Module Description:
    The implementation of TinyVkRenderer.

    Author:
    Zhao Han Ning

    Created Date:
    2022.4.18

    Notes:
    日了，Module 和 Macro 似乎并不能很好地混用？那么就得思考Conditonal Compilation如何在Module中使用
*/

#define WINDOW_APP
#define VK_USE_PLATFORM_WIN32_KHR

module;
#include <stdexcept>
#include <vulkan\vulkan.h>
module TinyVkRenderer;

import <vector>;


TinyVkRenderer::TinyVkRenderer(uint32_t windowWidth, uint32_t windowHeight)
    : surface(windowWidth, windowHeight)
{
    InitVulkanInstance();
    InitVulkanPhysicalDevices();
    InitVulkanLogicalDevice();
}

TinyVkRenderer::~TinyVkRenderer()
{
    vkDestroyDevice(vkDevice, nullptr);
    vkDestroyInstance(vkInstance, nullptr);
}

void TinyVkRenderer::Run() const
{
    while (surface.Update())
    {
        Render();
    }
}

void TinyVkRenderer::Render() const
{
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
    
    std::vector<VkQueueFamilyProperties> queueFamilyProperties;
    vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevices[0], &queueFamilyCount, queueFamilyProperties.data());
    
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
    createInfo.hinstance = surface.GetInstance();
    createInfo.hwnd = surface.GetHWDN();

    if (VK_SUCCESS != vkCreateWin32SurfaceKHR(vkInstance, &createInfo, nullptr, &vkSurface))
    {
        throw std::runtime_error("Failed to create Win32 Surface");
    }
#endif
}

void TinyVkRenderer::InitVulkanLogicalDevice()
{
    std::vector<const char*> enabledDeviceLayers;
    EnableDeviceLayers(enabledDeviceLayers);

    std::vector<const char*> enabledDeviceExtensions;
    EnableDeviceExtensions(enabledDeviceExtensions);

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfo(1);
    queueCreateInfo[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo[0].pNext = nullptr;
    queueCreateInfo[0].flags = 0;
    queueCreateInfo[0].queueFamilyIndex = 0;
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
    createInfo.enabledLayerCount = 0;
    createInfo.ppEnabledLayerNames = nullptr;
    createInfo.enabledExtensionCount = 0;
    createInfo.ppEnabledExtensionNames = nullptr;
    // TODO: 可以把Physical Device创建的feature直接拿来用，！但会增加性能开销！
    // 一个更加自然的实现就是用到什么就enable什么feature，但这个就比较复杂了……
    createInfo.pEnabledFeatures = nullptr;

    if (VK_SUCCESS != vkCreateDevice(vkPhysicalDevices[0], &createInfo, nullptr, &vkDevice))
    {
        throw std::runtime_error("Failed to create logical device");
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
    uint32_t extensionCount;
    std::vector<VkExtensionProperties> extensions;

    vkEnumerateDeviceExtensionProperties(vkPhysicalDevices[0], nullptr, &extensionCount, nullptr);
    if (extensionCount != 0)
    {
        extensions.resize(extensionCount);
        vkEnumerateDeviceExtensionProperties(vkPhysicalDevices[0], nullptr, &extensionCount, extensions.data());
    }
}
