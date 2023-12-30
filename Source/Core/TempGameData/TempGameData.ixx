/*
    Description:
    The interface of TempGameData module

    TempGameData 是用来临时放置所有游戏相关数据的地方

    Created Date:
    12/31/2023 12:19:57 AM
*/


export module TempGameData;

import Core;
import Camera;

namespace GameData
{

    export class TempGameData
    {
    public:
        TempGameData();

        void Update(float ms);

        std::shared_ptr<CameraRenderData> GetCameraRenderData() const;

    private:
        void CreateCamera();

        Camera camera;
    };

}