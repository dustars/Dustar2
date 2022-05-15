/*
    Description:
    The interface of VkComputePipeline module

    Created Date:
    5/15/2022 9:30:01 AM

    Notes:
*/

export module VkComputePipeline;

import Pipeline;

namespace RB
{

export class VkComputePipeline : public Pipeline
{
public:
    virtual PipelineType GetType() final override { return PipelineType::Compute; }
private:
	//??
};

}