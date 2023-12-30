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

import MiddleRenderer;
import Timer;
import Input;

import TempGameData;

export class SimpleApplication
{
public:
    SimpleApplication()
        : renderer(float2(1000, 800)),
          gameData()
    {
        renderer.SetCameraRenderData(gameData.GetCameraRenderData());

        // 还是有个顺序问题,需要最后Init Renderer,因为一些比如Camera数据需要先初始化,然后Renderer才拿来用
        renderer.Init();
    }
    ~SimpleApplication() {}

    void Run()
    {
        Timer::SetFramerate(60);
        while (renderer.WindowUpdate(Timer::GetMS()))
        {
            //如果限帧的话会出现大量问题……输入要和帧数系统解耦才行……
            //if (Timer::Tick())
            //{
                //TODO: Error return code handling
                Update(Timer::GetMS());

                //Input的更新不应该在这里被帧率限制
    			Input::InputManager::Execute();
                Render();
            //}
        }
    }

    bool Update(float ms)
    {
        gameData.Update(ms);
        return renderer.Update(ms);
    }
    bool Render()
    {
        return renderer.Render();
    }
private:
    // The order is important! Do not mess around!
    MiddleRenderer renderer;

    // 临时性的Game Data
    GameData::TempGameData gameData;
};