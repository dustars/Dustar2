/*
    Description:
    The interface of Camera module

    Created Date:
    5/15/2022 5:30:13 PM

    Notes:
*/

export module Camera;

import Core;

export struct CameraRenderData
{
	mat4	viewMatrix;
	mat4	orthMatrix;
	mat4	projMatrix;
};

export class Camera
{
	float	yaw;
	float	pitch;
	float3	position;
	float3	lookDir;
	float3	upDir;

	mat4	viewMatrix;
	mat4	projMatrix;
	mat4	orthMatrix;

public:
	Camera(float pitch = 0, float yaw = 0, float3 position = float3(0, 0, 0));
	~Camera(void) {};

	void BuildRenderCameraData();
	//Public Data for Renderer only
	std::shared_ptr<CameraRenderData> cameraRenderData;

	void UpdateCamera(float msec);

	mat4&	GetViewMatrix() { return viewMatrix; }
	void	BuildViewMatrix();
	
	mat4&	GetProjMatrix() { return projMatrix; }
	void	BuildProjMatrix(float znear, float zfar, float aspect, float fov);

	mat4&	GetOrthMatrix() { return orthMatrix; }
	void	BuildOrthogonalMatrix(float znear, float zfar, float right, float left, float top, float bottom);

	float3	GetPosition() const { return position; }
	void	SetPosition(float3 val) { position = val; }

	float	GetYaw()   const { return yaw; }
	void	SetYaw(float y) { yaw = y; }

	float	GetPitch() const { return pitch; }
	void	SetPitch(float p) { pitch = p; }

private:
	void CameraControlUpdate(float msec);
};