/*
    Description:
    The interface of ResourceLayout module

    Created Date:
    5/15/2022 8:42:29 PM

    Notes:
*/

export module RenderResource;

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

	virtual void CreateConstantBuffer() = 0;
	//virtual void CreateVertexBuffer();
	//virtual void CreateSRV();
	//virtual void CreateUAV();
	//virtual void CreateConstant();
};

}