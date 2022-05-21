/*
    Module Description:
    The interface of Module partition uint WindowSystem:WindowBase.
    Declare the abstract class window for all platforms.

    Created Date:
    2022.4.17
*/


export module WindowSystem:WindowBase;

namespace Window
{

export class WindowBase
{
public:

	//virtual void InitializeWindow() = 0;
	virtual bool Update(float)			const = 0;
};

}// namespace Window