/*
    Description:
    The interface of VkExtensions module

    Created Date:
    1/20/2024 11:42:57 AM
*/


export module VkExtensions;

import std;
import <vulkan\vulkan.h>;

// TODO:初始化Vulkan时创建, 创建完成后删掉!
export class VkExtensions
{
public:
    // Default enabled extensions
    // TODO: 后续或许可以保存为外部.ini文件之类的?
    VkExtensions()
    {
        configedExtensions.push_back("VK_KHR_swapchain");               // we are not doing offline rendering or some computing works, so...

        configedExtensions.push_back("VK_KHR_dynamic_rendering");       // vulkan 1.3 dynamic rendering
        configedExtensions.push_back("VK_KHR_synchronization2");        // vulkan 1.3 dynamic rendering

        configedExtensions.push_back("VK_EXT_descriptor_indexing");     // vulkan bindless feature
        configedExtensions.push_back("VK_KHR_buffer_device_address");   // vulkan bindless feature

        // Config features :<
        physicalDevicefeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
        physicalDevicefeatures.features.shaderInt64 = VK_TRUE;

        dynamicRenderingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES;
        dynamicRenderingFeatures.dynamicRendering = VK_TRUE;

        synchronization2Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES;
        synchronization2Features.synchronization2 = VK_TRUE;

        bufferDeviceAddressFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES;
        bufferDeviceAddressFeatures.bufferDeviceAddress = VK_TRUE;

        // Link the lisk
        dynamicRenderingFeatures.pNext = &physicalDevicefeatures;
        synchronization2Features.pNext = &dynamicRenderingFeatures;
        bufferDeviceAddressFeatures.pNext = &synchronization2Features;
        extensionFeaturesPtr = &bufferDeviceAddressFeatures;
    }

    uint32_t GetConfigedExtensionSize() { return configedExtensions.size(); }
    const char* const* GetConfigedExtensionData() { return configedExtensions.data(); }

    const void* GetEnabledFeaturesList() {return extensionFeaturesPtr;}

    bool CheckConfigedExtensionsAvailability(VkPhysicalDevice vkPhysicalDevice)
    {
        uint32_t extensionCount;
        std::vector<VkExtensionProperties> extsDevice;

        vkEnumerateDeviceExtensionProperties(vkPhysicalDevice, nullptr, &extensionCount, nullptr);
        if (extensionCount != 0)
        {
            extsDevice.resize(extensionCount);
            vkEnumerateDeviceExtensionProperties(vkPhysicalDevice, nullptr, &extensionCount, extsDevice.data());
        }

        for (auto ext : configedExtensions)
        {
            if (std::end(extsDevice) == std::find_if(
                std::begin(extsDevice),
                std::end(extsDevice),
                [&ext](const VkExtensionProperties& extDevice) {  return std::strcmp(extDevice.extensionName, ext) == 0; }
            )) return false;
        }
        return true;
    }

private:
    std::vector<const char*> configedExtensions;
    
    void* extensionFeaturesPtr;
    VkPhysicalDeviceFeatures2                       physicalDevicefeatures;
    VkPhysicalDeviceDynamicRenderingFeatures        dynamicRenderingFeatures;
    VkPhysicalDeviceSynchronization2Features        synchronization2Features;
    VkPhysicalDeviceDescriptorIndexingFeatures      descriptorIndexingFeatures;
    VkPhysicalDeviceBufferDeviceAddressFeatures     bufferDeviceAddressFeatures;
};