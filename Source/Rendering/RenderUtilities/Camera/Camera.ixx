/*
    Description:
    The interface of Camera module

    Created Date:
    5/15/2022 5:30:13 PM

    Notes:
*/

export module Camera;

import Math;

export class Camera
{
	float	yaw;
	float	pitch;
	float3	position;
	mat4	projMatrix;

public:
	Camera(float pitch = 0, float yaw = 360, float3 position = float3(0, 0, 0));
	~Camera(void) {};

	void UpdateCamera(float msec);

	mat4 BuildViewMatrix();
	
	mat4	GetProjMatrix() const { return projMatrix; }
	void	SetProjMatrix(mat4 projM) { projMatrix = projM; }

	float3	GetPosition() const { return position; }
	void	SetPosition(float3 val) { position = val; }

	float	GetYaw()   const { return yaw; }
	void	SetYaw(float y) { yaw = y; }

	float	GetPitch() const { return pitch; }
	void	SetPitch(float p) { pitch = p; }

private:
	void CameraControlUpdate(float msec);
};