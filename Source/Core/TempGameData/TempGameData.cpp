/*
    Description:
    The interface of TempGameData module

    TempGameData 是用来临时放置所有游戏相关数据的地方

    Created Date:
    12/31/2023 12:19:57 AM
*/

module TempGameData;

namespace GameData
{

    TempGameData::TempGameData()
    {
        CreateCamera();
    }

    void TempGameData::Update(float ms)
    {
        camera.UpdateCamera(ms);
    }

    std::shared_ptr<CameraRenderData> TempGameData::GetCameraRenderData() const
    {
        return camera.cameraRenderData;
    }

    void TempGameData::CreateCamera()
    {
        camera.SetYaw(0.f);
        camera.SetPitch(0.f);
        camera.SetPosition(float3(0.f, 0.f, 5.f));
        camera.BuildViewMatrix();
        camera.BuildProjMatrix(0.1f, 500.0f, 1000 / 800, 45.0f);
        camera.BuildOrthogonalMatrix(0.f, 10.f, -10.f, 10.f, 8.f, -8.f);
        camera.BuildRenderCameraData();
    }
}
