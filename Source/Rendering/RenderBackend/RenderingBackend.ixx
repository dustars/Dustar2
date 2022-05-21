/*
    Description:
    The interface of RenderingBackend module

    Created Date:
    5/10/2022 8:41:10 PM

    Notes:
    渲染后端，是Vulkan/D3D12/Metal三个现代图形API的抽象接口，为上层应用（目前主要是渲染器）
    提供各类简单创建接口，比如创建Pipeline，Buffer或Image等。

	TODO:
	1. 渲染资源模块 Resource?
	2. Graphics Pipeline 各种state数据如何设定？
	3. 渲染后端，智能CommandOps Lambda、Pipeline、RenderPass、Descriptor Set的调用框架
	4. 模型加载模块得到的渲染数据如何处理？（Vertex Buffer、Material Data）
	5. 创建一堆subPass和要用到的framebuffer(attachments) -> 自动处理依赖关系 -> 创建以RenderPass为单位的大循环结构（循环调用subpass，并Bind对应的pipeline)

    6. Shader自动编译: 写好的hlsl运行时动态编译
    
    我理想中对RBI的用法如下：
    1. 创建一个渲染器，如 MiddleRenderer
    2. 创建 Descriptor Layout (即该Pipeline会用到的各种资源)
    3. 如果是Graphics Pipeline, 就创建 FrameBuffer (compute没有)
    4. 将 Descriptor Layout 作为参数，创建 Pipeline 时传递
    5. 调用RHI->AddSubPass，将一个lambda注册到待执行列表中

    一些想法:
    Vulkan毕竟是错综复杂的，各个Object之间的创建互相依赖，这种天然的耦合让创建干净
    的C++类封装成为不可能，所以合理地互相引用，析构Objects又如何保证顺序，这些是必
    然要思考的问题

    在上层渲染逻辑中经常提到的pass, 严格对应到API, Vulkan中重合度最大的概念应该是subPass和Pipeline的组合

    保证Interface就是个接口，即编译C++后，接口不会产生任何*功能性*代码！
    
    在没有多线程命令录制之前，我永远都是vulkan的新手玩家
    在没有真正把同步引入之前，我永远都是vulkan的新手玩家
    在没有理解vulkan multithreading friendly之前，我永远都是vulkan的新手玩家

    资源:
    ResourceLayout是代表了一个Pipeline会用到的所有资源，需要在创建Pipeline前定义好，然后
    作为参数传递给Pipeline的创建函数。资源的类型、种类、数量会被自动统计，在渲染后端的
*/

export module RenderingBackend;

import CommonAbstractionClass;
import CmdBuffer;
import Pipeline;
import RenderResource;
import <functional>;
import <vector>;
import <string>;

namespace RB
{

export class RBInterface
{
    friend class RenderResourceManager;

    typedef std::function<void(CmdBuffer* cmd)> CmdOps;
    typedef std::vector<std::function<void(CmdBuffer* cmd)>> CmdOpsArray;
public:
    virtual ~RBInterface() {}

	virtual bool Update(float) = 0; //还不清楚具体的使用场景
	virtual bool Render() = 0;

    virtual Pipeline& CreateGraphicsPipeline(const ResourceLayout*, const ShaderArray&) = 0;
    virtual Pipeline& CreateComputePipeline(const ResourceLayout*, const ShaderFile&) = 0;

    void AddPass(CmdOps&& cmdOps)
    {
        renderingOps.push_back(move(cmdOps));
    }
    //需要FrameBuffer
    //VS和FS
    //各种管线配置的状态

    //virtual void CreateBuffer();
    //virtual void CreateImage();
    // View就是DX12里的SRV UAV?

    // 如果是Dispatch，势必意味着Compute Pipeline
    // 那么整体上会比图形管道简洁许多
    //virtual void CmdDispatch();
    // 如果是Draw，势必意味着会是一个Graphics Pipeline
    // 那就会有Vertex Buffer/Graphics Pipeline绑定
    //virtual void CmdDraw();
    //virtual void CmdDrawIndex();
    //virtual void CmdDrawIndirect();
    //virtual void CmdMultiDrawIndirect();

protected:
    CmdOpsArray renderingOps;

	virtual void InitResources(const std::vector<ResourceLayout*>& layouts) = 0;
	virtual ResourceLayout* CreateResourceLayout() = 0;
};

}