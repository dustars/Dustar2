/*
    Module Description:
    The implementation of TinyVkRenderer.

    Author:
    Zhao Han Ning

    Created Date:
    2022.4.18

    Last Updated:
    2022.4.22

    Notes:
*/

module;
#include <stdexcept>
module TinyVkRenderer;

import <vector>;

TinyVkRenderer::TinyVkRenderer(uint32_t windowWidth, uint32_t windowHeight)
    : surface(windowWidth, windowHeight)
{
    InitVulkanInstance();
    InitVulkanPhysicalDevices();
    InitVulkanLogicalDevice();
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
    VkApplicationInfo appInfo;
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pNext = nullptr;
    appInfo.pApplicationName = "Vulkan Learning";
    appInfo.applicationVersion = 1;
    appInfo.pEngineName = "NoEngine";
    appInfo.engineVersion = 0;
    appInfo.apiVersion = 1;

    VkInstanceCreateInfo createInfo;
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledLayerCount = 0;
    createInfo.ppEnabledLayerNames = nullptr;
    createInfo.enabledExtensionCount = 0;
    createInfo.ppEnabledExtensionNames = nullptr;
    
    if (VK_SUCCESS != vkCreateInstance(&createInfo, nullptr, &vkInstance))
    {
        throw std::runtime_error("failed to Create Vulkan Instance");
    }
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
}

void TinyVkRenderer::InitVulkanLogicalDevice()
{
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfo(1);
    queueCreateInfo[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo[0].pNext = nullptr;
    queueCreateInfo[0].flags = 0;
    queueCreateInfo[0].queueFamilyIndex = 0;
    queueCreateInfo[0].queueCount = 1;
    queueCreateInfo[0].pQueuePriorities = nullptr;

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