/*
    Module Description:
    The module primary interface for TinyVkREnderer.
    TinyRenderer is exclusively for learning Vulkan, thus
    very likely to be removed in the near future.

    Author:
    Zhao Han Ning

    Created Date:
    2022.4.17

    Last Updated:
    2022.4.22

    Notes:
*/

export module TinyVkRenderer;

import WindowSystem;

import <vector>;
import <vulkan\vulkan.h>;

//Create Window
//Create Vulkan Instance
//Do Vulkan Initializaton
//Render Triangle
//Continue loop

export class TinyVkRenderer
{
public:
	TinyVkRenderer(uint32_t windowWidth, uint32_t windowHeight);

    void Run()      const;
	
private:
    void Render()   const;

    

    Window::Win32Window surface;

    //Vulkan Initialization Objects
    VkInstance vkInstance;
    void InitVulkanInstance();

    std::vector<VkPhysicalDevice> vkPhysicalDevices;
    void InitVulkanPhysicalDevices();

    VkDevice vkDevice;
    void InitVulkanLogicalDevice();
    
};