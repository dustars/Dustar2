/*
    Description:
    The interface of RenderingBackend module

    Created Date:
    5/10/2022 8:41:10 PM

    Notes:
    渲染后端，是Vulkan/D3D12/Metal三个现代图形API的抽象接口，为上层应用（目前主要是渲染器）
    提供各类简单创建接口，比如创建Pipeline，Buffer或Image等。
    
    我理想中的用法如下：
    1. 创建一个渲染器。
    2. 在渲染器中各种做各种渲染特性，封装成Pass，然后整个渲染就是很多Pass的排列。
    3. 渲染器只负责创建如下资源：设置Pipeline的各种（根据图形/compute/RT划分），Shader Layout设定，
       Buffer/Image资源，其实和现在主流的都非常像……
    4. Pass是上层渲染管线的单个流程，其实并不和图形API有一一对应的概念（不是renderpass/subpass/GraphicsPipeline等）
    5. 所有Pass创建好后，是由系统负责解析每个Pass该被拆分到对应API的哪些概念上。

    我感觉自己走入了一个误区：如果用一层抽象包括了vkDevice vkInstance这类的Object，就是
    人为地制作一层屏障，让其他Vulkanobjects的创建非常不方便，这个问题如何解决呢？

    保证Interface就是个接口
    编译C++后，接口不会产生任何*功能性*代码！

    参数问题？？


    在没有多线程命令录制之前，我永远都是vulkan的新手玩家
    在没有真正把同步引入之前，我永远都是vulkan的新手玩家
    在没有理解vulkan multithreading friendly之前，我永远都是vulkan的新手玩家
*/

export module RenderingBackend;

import CmdBuffer;
import Pipeline;
import <functional>;
import <vector>;

namespace RB
{

export enum class RENDER_API
{
    VULKAN,
    D3D12,
    METAL,
    MAX
};

export class RBInterface
{
    typedef std::function<void(CmdBuffer* cmd)> CmdOps;
    typedef std::vector<std::function<void(CmdBuffer* cmd)>> CmdOpsArray;
public:
    virtual ~RBInterface() {}

	virtual bool Update() = 0; //还不清楚具体的使用场景
	virtual bool Render() = 0;

    virtual Pipeline& CreateGraphicsPipeline() = 0;
    virtual Pipeline& CreateComputePipeline() = 0;

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
};

}