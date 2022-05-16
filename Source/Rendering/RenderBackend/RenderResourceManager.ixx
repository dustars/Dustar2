/*
    Description:
    The interface of RenderResourceManager module

    Created Date:
    5/16/2022 9:56:05 PM

    Notes:
*/

export module RenderResourceManager;

import RenderingBackend;
import <vector>;

namespace RB
{

export class RenderResourceManager
{
    RBInterface* RBI;

    // 保存每个Layout的指针，做统一调度（同步主要是）
    // 由后端New，保存在Manager，但删除由每个绑定的Pipeline删除
    // （我知道这场指针的旅行很……野，但暂时我也没啥好办法
    // 主要是Manager目前设定为一次性的local var，在Renderer的Init()
    // 结束后就当场GG（析构），layout的所有权转移到Pipeilne里去了……
    std::vector<ResourceLayout*> layouts;

public:
	RenderResourceManager(RBInterface* RBI) : RBI(RBI) {}
    ~RenderResourceManager() { RBI->InitResources(layouts); }

    ResourceLayout* CreateResourceLayout()
    {
        layouts.push_back(RBI->CreateResourceLayout());
        return layouts.back();
    }
};

}