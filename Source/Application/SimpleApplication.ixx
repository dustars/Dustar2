/*
    Description:
    The interface of SimpleApplication module

    Created Date:
    5/10/2022 12:12:34 AM

    Notes:
*/

export module SimpleApplication;

import Timer;
import Input;
import WindowSystem;

import TempGameData;
import MiddleRenderer;

export class SimpleApplication
{
public:
    SimpleApplication() :
        window(1000, 800),
        renderer((void*)&window.GetHWDN()),
        gameData()
    {
        renderer.SetCameraRenderData(gameData.GetCameraRenderData());

        // 还是有个顺序问题,需要最后Init Renderer,因为一些比如Camera数据需要先初始化,然后Renderer才拿来用
        renderer.Init();
    }

    ~SimpleApplication() {}

    void Run()
    {
        //Configuration goes here, or some pre-executions to do
        Timer::SetFramerate(0);

        // Core loop of the application
        AppLoop();
    }

    bool AppLoop()
    {
        //TODO: 此时就应该Game Thread和render thread分开跑了, 以不同的帧率更新
        //代码类似如下:
        // if(satisfy gameRate) GameThread::Update()
        // ----Data Update----
        // if(satisfy renderRate) RenderThread::Update()

        //TODO: If window exists
        //Main loop
        while (window.Update(Timer::GetMS()))
        {
            double ms = Timer::GetMS();
            // window系统先处理输入
            Input::InputManager::Execute();
            // 游戏相关的临时数据更新
            gameData.Update(ms);
            // Post Loop functions 执行本帧(game?)的善后工作
            Input::InputManager::ResetMouse();


            // 渲染
            if (Timer::Tick()) renderer.Render();
        }

        return true;
    }

private:
    // Window
    // TODO: 隐藏鼠标
    Window::Win32Window window;

    // The order is important! Do not mess around!
    MiddleRenderer renderer;

    // 临时性的Game Data
    GameData::TempGameData gameData;
};