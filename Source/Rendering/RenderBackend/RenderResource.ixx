/*
    Description:
    The interface of ResourceLayout module

    Created Date:
    5/15/2022 8:42:29 PM

    Notes:
*/

export module RenderResource;

import Model;
import std;
import <stdint.h>;

namespace RB
{

export class Resource
{
public:
    ~Resource();
};

export class Buffer : public Resource
{

};

export class Image : public Resource
{

};

export class ResourceLayout
{
public:
    virtual ~ResourceLayout() {}

    virtual void CreateModelData(const Model&) = 0;
	virtual void CreatePushContant(const std::string&, uint32_t size, void* initData = nullptr) = 0;
	virtual void CreateConstantBuffer(const std::string&, uint32_t stride, uint32_t size, void* initData = nullptr) = 0;
	//virtual void CreateVertexBuffer(const std::string&, uint32_t stride, uint32_t size, void* initData = nullptr);
	//virtual void CreateSRV(const std::string&, uint32_t stride, uint32_t size, void* initData = nullptr);
	//virtual void CreateUAV(const std::string&, uint32_t stride, uint32_t size, void* initData = nullptr);
	//virtual void CreateConstant(const std::string&, uint32_t size, void* initData = nullptr);
};

}