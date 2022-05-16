/*
    Description:
    The implementation of Camera module

    Created Date:
    5/15/2022 5:34:20 PM
*/

module;

module Camera;

Camera::Camera(float pitch, float yaw, float3 position) :
	pitch(pitch),
	yaw(yaw),
	position(position)
{}

void Camera::UpdateCamera(float msec)
{

}

mat4 Camera::BuildViewMatrix()
{
	return	mat4::Rotation(-pitch, Vector3(1, 0, 0)) *
		mat4::Rotation(-yaw, Vector3(0, 1, 0)) *
		mat4::Translation(-position);
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