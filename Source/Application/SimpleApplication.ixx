/*
    Description:
    The interface of SimpleApplication module

    Created Date:
    5/10/2022 12:12:34 AM

    Notes:
*/

export module SimpleApplication;

import MiddleRenderer;
import <stdint.h>;

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
        while (Update() && Render())
        {
            //Do something?限帧之类的?
        }
    }

    bool Update()
    {
        return renderer.Update();
    }
    bool Render()
    {
        return renderer.Render();
    }
private:
    MiddleRenderer renderer;
};