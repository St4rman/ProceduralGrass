#pragma once
#include "Vector4.h"
#include "Vector3.h"

class Light{
public: 
	Light() {};
	Light(const Vector3& position, const Vector4& color, float radius) {
		this->position	= position;
		this->radius	= radius;
		this->color	= color;

	}
	~Light(void) {};

	Vector3 GetPosition() const { return position; }
	void SetPosition(const Vector3& val) { position = val; }
	
	float GetRadius() const { return radius; }
	void SetRadius(float val) { radius = val; }

	Vector4 GetColor() const { return color; }
	void SetColor(const Vector4& val) { color = val; }


protected: 
	Vector3 position;
	float	radius;
	Vector4 color;
};

