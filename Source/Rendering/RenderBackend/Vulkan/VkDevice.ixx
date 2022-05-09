/*
    Description:
    The interface of VkDevice module

    Created Date:
    5/9/2022 11:10:04 PM

    Notes:
*/

export module VkDevice;

import Device;
import <vector>;
import <vulkan\vulkan.h>;

namespace RB
{
class VulkanDevice : public Device
{
public:
	VulkanDevice();
private:
    
    VkInstance instance;
    std::vector<VkPhysicalDevice> physicalDevices;
    VkDevice device;

};
}