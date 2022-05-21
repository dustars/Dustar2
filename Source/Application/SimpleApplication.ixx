/*
    Description:
    The interface of SimpleApplication module

    Created Date:
    5/10/2022 12:12:34 AM

    Notes:
*/

export module SimpleApplication;

import MiddleRenderer;
import Timer;

export class SimpleApplication
{
public:
    SimpleApplication()
        : renderer()
    {
        renderer.Init();
    }
    ~SimpleApplication() {}

    void Run()
    {
        Timer::SetFramerate(0);
        do {
            while (!Timer::Tick());
        } while (Update(Timer::GetMS()) && Render());
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
    MiddleRenderer renderer;
};