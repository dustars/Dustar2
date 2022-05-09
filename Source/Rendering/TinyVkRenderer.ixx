/*
    Module Description:
    The module primary interface for TinyVkREnderer.
    TinyRenderer is exclusively for learning Vulkan, thus
    very likely to be removed in the near future.

	Created Date:
	2022.4.17

    Notes:
*/

export module TinyVkRenderer;

import WindowSystem;
import <string>;
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
    // Resources used by device will be updated by Application (no cmd involved)
    void Update();
    // Rendering operations. Recording command buffers and submit to the queue.
    void Render();

	void PreRender();
	void PostRender();

	/*********************************************************/
    /*-------------------------Window------------------------*/
    /*********************************************************/
    Window::Win32Window window;
    uint32_t windowWidth, windowHeight;
    VkSurfaceKHR vkSurface; //Surface refers to Vulkan's view of a window
	VkSwapchainKHR vkSwapChain; //SwapChain其实是跟窗口系统强关联的Object，内含N张Images，Vulkan会从里面请求一张可用的Image进行渲染，最后再present，可以说swapchain是Vulkan和外部窗口系统的接口了
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;
	std::vector<VkImage> vkSwapChainImages; //存储在SwapChain中的N张Images
    void InitWindowSurface();
	void InitSwapChain();
	uint32_t GetAvailableImage(uint64_t waitTimeNano); //从窗口系统获取一张可用的Image

    /*********************************************************/
    /*---------------------Initialization--------------------*/
    /*********************************************************/
    VkInstance vkInstance;
    std::vector<VkPhysicalDevice> vkPhysicalDevices;
    VkDevice vkDevice;

    void InitVulkanInstance();
    void InitVulkanPhysicalDevices();
    void InitVulkanLogicalDevice();

	void EnableInstanceLayers(std::vector<const char*>&);
	void EnableInstanceExtensions(std::vector<const char*>&);
    void EnableDeviceLayers(std::vector<const char*>&);
    void EnableDeviceExtensions(std::vector<const char*>&);

	/*********************************************************/
    /*--------------------Command Buffers--------------------*/
    /*********************************************************/
    // Queue and Command Buffers
    VkCommandPool vkCommandPool;
    std::vector<VkQueue> vkQueues;
    uint32_t currentQueueFamilyIndex;
    VkCommandBuffer cmd; // 之后可以每个线程拥有一个cmd，乃至于command Pool
    void InitCommandPool();
    void InitCommandBuffers();

    void BeginCommandBuffer();
    void EndCommandBuffer();
    void SubmitCommandBuffer();

    /*********************************************************/
    /*-----------------------Resources-----------------------*/
    /*********************************************************/
    // 用于学习一些资源相关的API，会把用法暂时放在里面，后续会删掉
    void Temp();
    void CheckFormatProperties();

    VkDeviceMemory deviceMem;
    void AllocateMemory(uint32_t memoryType);
    uint32_t FindMemoryType(const VkMemoryRequirements& memoryRequirements, VkMemoryPropertyFlags requiredFlags, VkMemoryPropertyFlags preferredFlags);  

    VkDescriptorSetLayout descriptorSetLayout;
    VkDescriptorPool descriptorPool;
    std::vector<VkDescriptorSet> descriptorSet;
    void CreateDescriptorSetLayout();
    void CreateDescriptorPool();
    void AllocateDescriptorSets();
    void UpdateDescriptorSets();

    /*********************************************************/
    /*-------------------------Shader------------------------*/
    /*********************************************************/
    // Shader System啊……估计也会非常复杂吧到了后期
    // TODO: 居然书中多次提到Vulkan本身并不关心Shader Module是如何来的，
    // 也就是说我要把SPIR-V的Binary Code加载到App里，Byte Size和一个指向
    // Binary code 的 pointer，麻烦了啊艹
    VkShaderModule shaderModule;
    VkPipeline computePipeline;
    VkPipelineLayout pipelineLayout;
    VkPipelineCache pipelineCache = VK_NULL_HANDLE;
    void CreateShaderModule(const std::string& shaderFile, VkShaderModule& mod);
    void CreateComputePipeline();
    void CreatePipelineLayout();
    void CreatePipelineCache();

private: //data
    uint64_t memSize;


	/*********************************************************/
	/*-----------------------Playground----------------------*/
	/*********************************************************/
    // 将一张图片的颜色反转
    // 创建Pipeline/Shader/Descriptor Set/Image/Image View/Sampler 反正所有要用到的资源，算是第一次实战
    // 注意写的是HLSL，要看一下和Vulkan之间的Mapping问题： https://github.com/microsoft/DirectXShaderCompiler/blob/master/docs/SPIR-V.rst
    void InvertImageInit();
    void InvertImageResourceClean();
    void InvertImageRender();
    void InvertImageUpdate();

    void CreateSrcSampler();
	void CreateSrcImage(const std::string& filename);
	void CreateDstImage();
    void CreateSrcImageView();
    void CreateDstImageView();

    VkSampler srcSampler;
    VkImage srcImage;
    VkDeviceMemory srcMemory;
    VkImage dstImage;
    VkDeviceMemory dstMemory;

    VkImageView srcImageView;
    VkImageView dstImageView;

    typedef struct Vertex
    {
        float x,y,z,w;
        float u,v;
    };
    std::vector<Vertex> vertexData =
    {
        {0.f, 0.5f, 0.f, 1.f, 0.5f, 1.f},
        {-0.5f, -0.5f, 0.f, 1.f, 0.f, 0.f},
        {0.5f, -0.5f, 0.f, 1.f, 1.f, 0.f}
    };

    // 输出该图片
    VkShaderModule vertShader;
    VkShaderModule fragShader;

    VkBuffer vertexBuffer;
    VkDeviceMemory vertexMemory;
    std::vector<VkImageView> presentImageViews;
    VkSemaphore imageAvailableSemaphore;
    VkSemaphore renderFinishedSemaphore;
    VkFence inFlightFence;
    VkRenderPass renderPass;
    std::vector<VkFramebuffer> framebuffers;
    VkPipelineLayout graphicsPipelineLayout;
    VkPipeline graphicsPipeline;
    void PresentImageInit();
    void PresentImageClean();
    void PresentImageRender();

    void CreateSynchronizationObjects();
    void CreateVertexBuffer();
    void CreatePresentImageView();
    void CreateRenderPass();
    void CreateFramebuffer();
    void CreateGraphicsPipeline();

    void CmdBeginRenderPass(VkFramebuffer& framebuffer);
    void CmdEndRenderPass();
    void CmdDraw();
    void ImagePresentation(uint32_t imageIndex);

	/*********************************************************/
    /*-----------------------Utilities-----------------------*/
    /*********************************************************/
    bool needCapture = true;
};

//class GraphicsPipeline : Pipeline {
//	CreatePipeline();
//	CreateResources(); ..
//};