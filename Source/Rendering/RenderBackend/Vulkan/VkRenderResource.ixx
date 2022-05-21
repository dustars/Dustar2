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
import Model;
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
	VkPhysicalDevice vkPhysicalDevice;

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

	struct resEntry
	{
		std::string name;
		uint32_t size;
		std::variant<VkBuffer, VkImage> res;
		void* data;
		resEntry(std::string name, uint32_t size, std::variant<VkBuffer, VkImage> res, void* data)
			: name(name), size(size), res(res), data(data) {}
	};

    std::vector<std::string> resourceNames;
    VkDescriptorSetLayout setLayout;
    VkDescriptorSet set;

    VkPipelineLayout pipelineLayout;
    struct PushConstant
    {
        std::string name; void* data;
		PushConstant(std::string name, void* data) : name(name), data(data) {}
    };
    std::vector<PushConstant> pushConstants;

    //Model Data
    VkBuffer vertexBuffer;
    VkDeviceMemory vertexMemory;
    VkBuffer indexBuffer;
    VkDeviceMemory indexMemory;
    uint32_t vertexCount;
    uint32_t indexCount;
public:
    VkResourceLayout(VkDevice device, VkPhysicalDevice pDev) : vkDevice(device) , vkPhysicalDevice(pDev) {}
    virtual ~VkResourceLayout();

    virtual void CreateModelData(const Model&) final override;
    virtual void CreatePushContant(const std::string&, uint32_t, void* = nullptr) final override;
    virtual void CreateConstantBuffer(const std::string&, uint32_t, uint32_t, void* = nullptr) final override;

	VkPipelineLayout GetPipelineLayout() { return pipelineLayout; }
	const VkDescriptorSet* GetDescriptorSet() { return &set; }

    uint32_t GetPushConstantsSize() { return pushConstants.size(); }
    void*    SetPushConstant(uint32_t, VkShaderStageFlags&, uint32_t&);
	const VkBuffer* GetVertexBufferPtr() { return &vertexBuffer; }
	const VkBuffer* GetIndexBufferPtr() { return &indexBuffer; }
	uint32_t GetVertexCount() { return vertexCount; }
	uint32_t GetIndexCount() { return indexCount; }
private:
    // Called when creating Graphics Pipeline
	VkPipelineLayout BuildPipelineLayout();

    // Called Before execution of command buffer.
    void AllocateDescriptorSet();
    void BindResourcesAndDescriptors();

    // Utilities

// Static Members:
private:
    static std::vector<ResourceInfo> resourceInfo;
    static const ResourceInfo* FindResource(const std::string& name)
    {
        for (auto& entry : resourceInfo) if (entry.name == name) return &entry;
        return nullptr;
    }

	static std::unordered_map<VkDescriptorType, uint32_t> poolMembers;
	static VkDescriptorPool descriptorPool;
    static void CreateDescriptorPool(VkDevice);

    static uint32_t FindMemoryType(VkPhysicalDevice, const VkMemoryRequirements&, VkMemoryPropertyFlags, VkMemoryPropertyFlags);

private:
    // 暂时把Buffer/Image相关放在这里
    static VkMemoryRequirements bufferMemoryRequirements;
	static VkDeviceMemory bufferMemory;
    static VkMemoryRequirements imageMemoryRequirements;
	static VkDeviceMemory ImageMemory;

    static std::vector<resEntry> buffers;
    static std::vector<resEntry> Images;
	void CreateBuffer(const std::string&, uint32_t, void*);
	void CreateImage(const std::string&);

    static void CreateResources(VkDevice, VkPhysicalDevice);
    // Called by CreateResources()
    static void CreateMemory(VkDevice, VkPhysicalDevice);
    static void BindBuffersAndMemory(VkDevice);
    static void BindImagesAndMemory(VkDevice);
};

}