/*
    Description:
    The interface of Pipeline module

    Created Date:
    5/15/2022 9:54:55 AM

    Notes:
*/

export module Pipeline;

namespace RB
{

export enum PipelineType
{
    Graphics,
    Compute,
    RayTracing
};

export class Pipeline
{
public:
	virtual ~Pipeline() {}
    virtual PipelineType GetType() = 0;
};

}