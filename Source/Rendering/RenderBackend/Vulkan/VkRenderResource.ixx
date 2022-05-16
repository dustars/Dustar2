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
import <vulkan\vulkan.h>;

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
    struct BindingInfo
    {
        VkDescriptorType type;
        uint32_t size;
    };

    friend class VkRenderingBackend;
    friend class VkGraphicsPipeline;

    static VkDescriptorPool descriptorPool;

    std::vector<BindingInfo> bindingInfo;
    std::vector<VkDescriptorSetLayoutBinding> bindings;
    VkDescriptorSetLayout setLayout;
    VkPipelineLayout pipelineLayout;
public:
    ~VkResourceLayout();

    virtual void CreateConstantBuffer() final override;
    
private:
    static void CreateDescriptorPool();

    void AllocateDescriptorSet();
    void CreateResources();
    void BindResourcesAndDescriptors();

	VkPipelineLayout BuildPipelineLayout(VkDevice);
    void DestroyResource(VkDevice);
};

}