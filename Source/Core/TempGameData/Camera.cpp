/*
    Description:
    The implementation of Camera module

    Created Date:
    5/15/2022 5:34:20 PM
*/

module;
#define FACTOR 0.01f
#include <math.h>;
module Camera;

Camera::Camera(float pitch, float yaw, float3 position) :
	pitch(pitch),
	yaw(yaw),
	position(position),
	cameraRenderData(std::make_shared<CameraRenderData>())
{
	BuildViewMatrix();
}

void Camera::BuildRenderCameraData()
{
	cameraRenderData->viewMatrix = viewMatrix;
	cameraRenderData->orthMatrix = orthMatrix;
	cameraRenderData->projMatrix = projMatrix;
}

void Camera::UpdateCamera(double msec)
{
	yaw += 0.5 * FACTOR * Input::InputManager::GetMouseXOffset();
	pitch -= 0.5 * FACTOR * Input::InputManager::GetMouseYOffset();

	if (Input::InputManager::GetKeyPressed(Input::Bindings::W)) {
		position += (Matrix4::Rotation(yaw, Vector3(0, 1, 0)) * Vector3(0, 0, -1) * msec) * FACTOR;
	}
	if (Input::InputManager::GetKeyPressed(Input::Bindings::S)) {
		position -= (Matrix4::Rotation(yaw, Vector3(0, 1, 0)) * Vector3(0, 0, -1) * msec) * FACTOR;
	}

	if (Input::InputManager::GetKeyPressed(Input::Bindings::A)) {
		position += (Matrix4::Rotation(yaw, Vector3(0, 1, 0)) * Vector3(-1, 0, 0) * msec) * FACTOR;
	}
	if (Input::InputManager::GetKeyPressed(Input::Bindings::D)) {
		position -= (Matrix4::Rotation(yaw, Vector3(0, 1, 0)) * Vector3(-1, 0, 0) * msec) * FACTOR;
	}

	if (Input::InputManager::GetKeyPressed(Input::Bindings::SPACE)) {
		position.y -= msec * FACTOR;
	}
	if (Input::InputManager::GetKeyPressed(Input::Bindings::L_SHIFT)) {
		position.y += msec * FACTOR;
	}

	// TODO: Should I put it into another method?
	BuildViewMatrix();
	// Update Render Data
	BuildRenderCameraData();
}

void Camera::BuildViewMatrix()
{
	viewMatrix =
		mat4::Rotation(-pitch, Vector3(1, 0, 0)) *
		mat4::Rotation(-yaw, Vector3(0, 1, 0)) *
		mat4::Translation(-position);
}

void Camera::BuildProjMatrix(float znear, float zfar, float aspect, float fov)
{
	projMatrix.ToIdentity();
	
	const float h = 1.0f / tan(fov * PI_OVER_360);
	float neg_depth = znear - zfar;

	projMatrix.values[0] = h / aspect;
	projMatrix.values[5] = h;
	projMatrix.values[10] = (zfar + znear) / neg_depth;
	projMatrix.values[11] = -1.0f;
	projMatrix.values[14] = 2.0f * (znear * zfar) / neg_depth;
	projMatrix.values[15] = 0.0f;
}

void Camera::BuildOrthogonalMatrix(float znear, float zfar, float right, float left, float top, float bottom)
{
	orthMatrix.ToIdentity();

	orthMatrix.values[0] = 2.0f / (right - left);
	orthMatrix.values[5] = 2.0f / (top - bottom);
	orthMatrix.values[10] = -2.0f / (zfar - znear);

	orthMatrix.values[12] = -(right + left) / (right - left);
	orthMatrix.values[13] = -(top + bottom) / (top - bottom);
	orthMatrix.values[14] = -(zfar + znear) / (zfar - znear);
	orthMatrix.values[15] = 1.0f;
}

void Camera::CameraControlUpdate(float msec)
{
	//if (Window::GetMouse()->ButtonHeld(MouseButtons::MOUSE_LEFT)) {
	//	sunZenith += (Window::GetMouse()->GetRelativePosition().y);
	//	sunAzimuth -= (Window::GetMouse()->GetRelativePosition().x);

	//	if (sunZenith < 0) sunZenith += 360.0f;
	//	if (sunZenith > 360.0f) sunZenith -= 360.0f;
	//	//sunZenith = min(sunZenith, 0.0f);
	//	//sunZenith = max(sunZenith, 180.0f);
	//	if (sunAzimuth < 0) sunAzimuth += 360.0f;
	//	if (sunAzimuth > 360.0f) sunAzimuth -= 360.0f;
	//}
	//else {
	//	pitch -= (Window::GetMouse()->GetRelativePosition().y);
	//	yaw -= (Window::GetMouse()->GetRelativePosition().x);
	//}

	////Bounds check the pitch, to be between straight up and straight down ;)
	//pitch = min(pitch, 90.0f);
	//pitch = max(pitch, -90.0f);

	//if (yaw < 0) {
	//	yaw += 360.0f;
	//}
	//if (yaw > 360.0f) {
	//	yaw -= 360.0f;
	//}

	//if (Window::GetKeyboard()->KeyDown(KEYBOARD_W)) {
	//	position += (Matrix4::Rotation(yaw, Vector3(0, 1, 0)) * Vector3(0, 0, -1) * msec) / UPDATE_MULTIPLIER;
	//}
	//if (Window::GetKeyboard()->KeyDown(KEYBOARD_S)) {
	//	position -= (Matrix4::Rotation(yaw, Vector3(0, 1, 0)) * Vector3(0, 0, -1) * msec) / UPDATE_MULTIPLIER;
	//}

	//if (Window::GetKeyboard()->KeyDown(KEYBOARD_A)) {
	//	position += (Matrix4::Rotation(yaw, Vector3(0, 1, 0)) * Vector3(-1, 0, 0) * msec) / UPDATE_MULTIPLIER;
	//}
	//if (Window::GetKeyboard()->KeyDown(KEYBOARD_D)) {
	//	position -= (Matrix4::Rotation(yaw, Vector3(0, 1, 0)) * Vector3(-1, 0, 0) * msec) / UPDATE_MULTIPLIER;
	//}

	//if (Window::GetKeyboard()->KeyDown(KEYBOARD_SPACE)) {
	//	position.y += msec / UPDATE_MULTIPLIER;
	//}
	//if (Window::GetKeyboard()->KeyDown(KEYBOARD_SHIFT)) {
	//	position.y -= msec / UPDATE_MULTIPLIER;
	//}
}