/*
    Description:
    The interface of SimpleApplication module

    Created Date:
    5/10/2022 12:12:34 AM

    Notes:
*/

module;
#define RENDER_DOC_ENABLE
export module SimpleApplication;

import VkRenderingBackend;
import RenderDocPlugin;
import WindowSystem;
import MiddleRenderer;
import Timer;
import Input;

export class SimpleApplication
{
public:
    SimpleApplication()
        : window(800, 600)
        , renderDoc(RB::VkRBInterface::GetVkInstance(), (void*)&window.GetHWDN())
		, renderer()
    {
        renderer.Init();
    }
    ~SimpleApplication() {}

    void Run()
    {
        Timer::SetFramerate(60);
        while (window.Update(Timer::GetMS()))
        {
            if (Timer::Tick())
            {
                //TODO: Error return code handling
                Update(Timer::GetMS());
                Render();
            }
			Input::InputManager::Execute();
        }
    }

    bool Update(float ms)
    {
        return renderer.Update(ms);
    }
    bool Render()
    {
        return renderer.Render();
    }
private:
    // The order is important! Do not mess around!
	Window::Win32Window window;
    RenderDocPlugin renderDoc;
    MiddleRenderer renderer;
};