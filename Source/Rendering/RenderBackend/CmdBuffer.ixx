/*
    Description:
    The interface of CmdBuffer module

    Created Date:
    5/15/2022 9:05:49 AM

    Notes:
*/

export module CmdBuffer;

import Pipeline;

namespace RB
{

export class CmdBuffer
{
public:
    virtual ~CmdBuffer() {}

    virtual void Draw(Pipeline&) = 0;
};

}