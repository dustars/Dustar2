/*
    Description:
    The interface of VkResource module

    Created Date:
    5/16/2022 9:13:41 PM

    Notes:
    1. 目前进行到渲染后端的InitResources()函数, 还差Pool, allocate
    Image/Buffer/Memory封装，以及两者的绑定，最后就是Draw了!
*/

export module VkRenderResource;

import RenderResource;
import <vector>;
import <string>;
import <unordered_map>;
import <vulkan\vulkan.h>;
import <variant>;

namespace RB
{


export class VulkanBuffer : public Buffer
{

};

export class VulkanImage : public Image
{

};

export class VkResourceLayout : public ResourceLayout
{
	friend class VkRBInterface;
	friend class VkGraphicsPipeline;

	// Reference Handle
	VkDevice vkDevice;

private:
    struct ResourceInfo
    {
        std::string name;
        VkDescriptorType type;
        VkShaderStageFlags stage;
        uint32_t stride;
        uint32_t size;

        ResourceInfo(
            const std::string& name,
            VkDescriptorType type,
            VkShaderStageFlags stage,
            uint32_t stride,
            uint32_t size)
        : name(name), type(type), stage(stage), stride(stride), size(size) {}
    };

    std::vector<std::string> resourceNames;
    VkDescriptorSetLayout setLayout;
    VkDescriptorSet set;

    VkPipelineLayout pipelineLayout;
public:
    VkResourceLayout(VkDevice device) : vkDevice(device) {}
    virtual ~VkResourceLayout() { DestroyResource(); }

    virtual void CreateConstantBuffer(const std::string&, uint32_t, uint32_t, void* = nullptr) final override;


	VkPipelineLayout GetPipelineLayout() { return pipelineLayout; }
	const VkDescriptorSet* GetDescriptorSet() { return &set; }
private:
    void AllocateDescriptorSet();
    void UpdateDescriptorSet();
    void BindResourcesAndDescriptors();

	VkPipelineLayout BuildPipelineLayout();
    void DestroyResource();

    // Utilities
    static uint32_t FindMemoryType(VkPhysicalDevice, const VkMemoryRequirements&, VkMemoryPropertyFlags, VkMemoryPropertyFlags);

// Static Members:
private:
	static std::unordered_map<VkDescriptorType, uint32_t> poolMembers;
	static VkDescriptorPool descriptorPool;

    static std::vector<ResourceInfo> resourceInfo;

    static void CreateDescriptorPool(VkDevice);

private:
    // 暂时把Buffer/Image相关放在这里
    static VkMemoryRequirements bufferMemoryRequirements;
	static VkDeviceMemory bufferMemory;
    static VkMemoryRequirements imageMemoryRequirements;
	static VkDeviceMemory ImageMemory;

    struct resEntry
    {
        std::string name;
        uint32_t size;
        std::variant<VkBuffer, VkImage> res;
        void* data;
        resEntry(std::string name, uint32_t size, std::variant<VkBuffer, VkImage> res, void* data)
            : name(name), size(size), res(res), data(data) {}
    };
    static std::vector<resEntry> buffers;
    static std::vector<resEntry> Images;

	void CreateBuffer(const std::string&, uint32_t, void*);
	void CreateImage(const std::string&);

    static void CreateResources(VkDevice, VkPhysicalDevice);

    static void CreateMemory(VkDevice, VkPhysicalDevice);
    static void BindBuffersAndMemory(VkDevice);
    static void BindImagesAndMemory(VkDevice);
};

}