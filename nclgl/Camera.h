#pragma once
#include "Matrix4.h"
#include "Matrix3.h"

class Camera
{
public:
	Camera(void) {
		yaw = 0.0f;
		pitch = 0.0f;
		roll = 0.0f;
		//speed = 0.0f;
	};

	inline Camera(float pitch, float yaw, Vector3 position) {
		this->pitch = pitch;
		this->yaw = yaw;
		this->position = position;
		roll = 0.0f;
	}

	inline Camera(float pitch, float yaw, float roll, Vector3 position) {
		this->pitch = pitch;
		this->yaw = yaw;
		this->roll = roll;
		this->position = position;
	}

	~Camera(void) { };

	void UpdateCamera(float dt = 1.0f);

	Matrix4 BuildViewMatrix();

	Vector3 GetPosition() const { return position; }
	void SetPosition(Vector3 val) { position = val; }

	float getYaw() const { return yaw; }
	void setYaw(float val) { yaw = val; }

	float getPitch() const { return pitch; }
	void setPitch(float val) { pitch = val; }

	float getRoll() const { return roll; }
	void setRoll(float val) { roll = val; }


protected:
	float speed;
	float yaw;
	float pitch;
	float roll;
	Vector3 position;
};