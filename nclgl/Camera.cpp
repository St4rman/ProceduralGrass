#include "Camera.h"
#include "Window.h"
#include <algorithm>

void Camera::UpdateCamera(float dt) {

	pitch -= (Window::GetMouse()->GetRelativePosition().y);
	yaw -= (Window::GetMouse()->GetRelativePosition().x);
	speed = 1000.0f * dt;

	//mathf.clamp basicallys
	pitch = std::min(pitch, 90.0f);
	pitch = std::max(pitch, -90.0f);


	//checking for negative cases and then checking if theyre left or right
	if (yaw < 0) {
		yaw += 360.0f;
	}
	if (yaw > 360.0f) {
		yaw -= 360.0f;
	}

	//getting the value of rotation from value of mouse around the axis
	Matrix4 rotation = Matrix4::Rotation(yaw, Vector3(0, 1, 0));

	//Getting directions from the new rotation
	Vector3 forward = rotation * Vector3(0, 0, -1);
	Vector3 right = rotation * Vector3(1, 0, 0);


	if (Window::GetKeyboard()->KeyDown(KEYBOARD_W)) {
		position += forward * speed;
	}if (Window::GetKeyboard()->KeyDown(KEYBOARD_A)) {
		position -= right * speed;
	}if (Window::GetKeyboard()->KeyDown(KEYBOARD_S)) {
		position -= forward * speed;
	}if (Window::GetKeyboard()->KeyDown(KEYBOARD_D)) {
		position += right * speed;
	}if (Window::GetKeyboard()->KeyHeld(KEYBOARD_SHIFT)) {
		speed *= 20.0f;
	}if (Window::GetKeyboard()->KeyDown(KEYBOARD_SPACE)) {
		position.y += speed;
	}if (Window::GetKeyboard()->KeyDown(KEYBOARD_CONTROL)) {
		position.y -= speed;
	}


};


Matrix4 Camera::BuildViewMatrix() {

	//sets the camera i think 
	return Matrix4::Rotation(-pitch, Vector3(1, 0, 0)) *
		Matrix4::Rotation(-yaw, Vector3(0, 1, 0)) *
		Matrix4::Rotation(roll, Vector3(0, 0, 1)) *
		Matrix4::Translation(-position);
}