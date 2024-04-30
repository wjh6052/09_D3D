#pragma once

class Freedom : public Camera
{
public:
	Freedom() = default;
	~Freedom() = default;


	void Update() override;

	void Speed(float moveSpeed, float rotationSpeed = 10.0f);

private:
	float moveSpeed = 20.f;
	float rotationSpeed = 10.f;
};