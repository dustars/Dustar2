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

    void Run();
	
private:
    void Render();

    Window::Win32Window window;

    /*********************************************************/
    /*---------------------Initialization--------------------*/
    /*********************************************************/
    VkInstance vkInstance;
    void InitVulkanInstance();
    void EnableInstanceLayers(std::vector<const char*>&);
    void EnableInstanceExtensions(std::vector<const char*>&);

    std::vector<VkPhysicalDevice> vkPhysicalDevices;
    void InitVulkanPhysicalDevices();
    VkSurfaceKHR vkSurface; //Surface refers to Vulkan's view of a window
    void InitWindow();
    
    VkDevice vkDevice;
    void InitVulkanLogicalDevice();
    void EnableDeviceLayers(std::vector<const char*>&);
    void EnableDeviceExtensions(std::vector<const char*>&);

    VkSwapchainKHR vkSwapChain; //SwapChain其实是跟窗口系统强关联的Object，内含N张Images，Vulkan会从里面请求一张可用的Image进行渲染，最后再present，可以说swapchain是Vulkan和外部窗口系统的接口了
    std::vector<VkImage> vkSwapChainImages; //存储在SwapChain中的N张Images
    void InitSwapChain();
    uint32_t GetAvailableImage(uint64_t waitTimeNano); //从窗口系统获取一张可用的Image

    // Queue and Command Buffers
    std::vector<VkQueue> vkQueues;
    uint32_t currentQueueFamilyIndex;
    VkCommandPool vkCommandPool;
    // 之后可以每个线程拥有一个cmd，乃至于command Pool
    VkCommandBuffer cmd;
    void InitCommandPool();
    void InitCommandBuffers();

    /*********************************************************/
    /*-----------------Rendering Operations------------------*/
    /*********************************************************/
    void PreRender();
    void PostRender();

    //TODO: Recycling command buffers
    void BeginCommandBuffer();
    void EndCommandBuffer();
    void SubmitCommandBuffer();
};