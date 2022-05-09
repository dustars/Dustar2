/*
    Description:
    The interface of SimpleApplication module

    Created Date:
    5/10/2022 12:12:34 AM

    Notes:
*/

export module SimpleApplication;

import TinyVkRenderer;
import MiddleRenderer;
import <stdint.h>;

export class SimpleApplication
{
public:
    SimpleApplication(uint32_t width, uint32_t height)
        : renderer(width, height)
        //, renderer(?)
    {
        //renderer.Init();
    }
    ~SimpleApplication() {}

    void Run()
    {
        renderer.Run();
        Update();
        Render();
    }

    void Update()
    {
        //renderer.Update();
    }
    void Render()
    {
        //renderer.Render();
    }
private:
    TinyVkRenderer renderer;
    //MiddleRenderer renderer;
};