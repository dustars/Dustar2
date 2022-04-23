/*
    Module Description:
    The module primary interface for TinyVkREnderer.
    TinyRenderer is exclusively for learning Vulkan, thus
    very likely to be removed in the near future.

    Author:
    Zhao Han Ning

    Created Date:
    2022.4.17

    Notes:
*/

export module TinyVkRenderer;

import WindowSystem;
import <vector>;
import <vulkan\vulkan.h>;

export class TinyVkRenderer
{
public:
	TinyVkRenderer(uint32_t windowWidth, uint32_t windowHeight);
    ~TinyVkRenderer();

    //TinyVkRenderer(const TinyVkRednerer&) = delete; //????
    TinyVkRenderer& operator=(const TinyVkRenderer& other) = delete;

    void Run()      const;
	
private:
    void Render()   const;

    Window::Win32Window surface;
    VkSurfaceKHR vkSurface;

    //Vulkan Initialization Objects
    VkInstance vkInstance;
    void InitVulkanInstance();
    void EnableInstanceLayers(std::vector<const char*>&);
    void EnableInstanceExtensions(std::vector<const char*>&);

    std::vector<VkPhysicalDevice> vkPhysicalDevices;
    void InitVulkanPhysicalDevices();
    void InitWindow();

    VkDevice vkDevice;
    void InitVulkanLogicalDevice();
    void EnableDeviceLayers(std::vector<const char*>&);
    void EnableDeviceExtensions(std::vector<const char*>&);
};