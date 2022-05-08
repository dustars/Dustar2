export module WindowSystem:WindowBase;

namespace Window
{

export class WindowBase
{
public:

	//virtual void InitializeWindow() = 0;
	virtual bool Update()			const = 0;
};

}// namespace Window