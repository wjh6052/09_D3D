#include "Framework.h"
#include "Camera.h"


Camera::Camera()
{
	D3DXMatrixIdentity(&matRotation);
	D3DXMatrixIdentity(&matView);

	Rotation();
	Move();
}


void Camera::Position(float x, float y, float z)
{
	Position(Vector3(x, y, z));
}

void Camera::Position(Vector3& vec)
{
	position = vec;

	Move();
}

void Camera::Position(Vector3* vec)
{
	*vec = position;
}


void Camera::Rotation(float x, float y, float z)
{
	Rotation(Vector3(x, y, z));
}

void Camera::Rotation(Vector3& vec)
{
	rotation = vec;

	Rotation();
}

void Camera::Rotation(Vector3* vec)
{
	*vec = rotation;
}


void Camera::RotationDegree(float x, float y, float z)
{
	RotationDegree(Vector3(x, y, z));
}

void Camera::RotationDegree(Vector3& vec)
{
	rotation = vec * 0.017453f;
}

void Camera::RotationDegree(Vector3* vec)
{
	*vec = rotation * 57.29579f;
}


void Camera::GetMatrix(Matrix* matrix)
{
	*matrix = matView;

	//Matrix m = matView; //-> ���� ������ ȣ��
	//memcpy(&m, &matView, sizeof(Matrix)); // XXXX
}


void Camera::Rotation()
{
	//Matrix Pitch, Yaw, Roll;
	//D3DXMatrixRotationX(&Pitch, rotation.x);
	//D3DXMatrixRotationY(&Yaw, rotation.y);
	//D3DXMatrixRotationZ(&Roll, rotation.z);
	//matRotation = Pitch * Yaw * Roll;
	//world = S * R * T

	D3DXMatrixRotationYawPitchRoll(&matRotation, rotation.y, rotation.x, rotation.z);

	D3DXVec3TransformNormal(&forward, &Vector3(0, 0, 1), &matRotation);
	D3DXVec3TransformNormal(&right, &Vector3(1, 0, 0), &matRotation);
	D3DXVec3TransformNormal(&up, &Vector3(0, 1, 0), &matRotation); 

}


void Camera::Move()
{
	View();
}


void Camera::View()
{
	D3DXMatrixLookAtLH(&matView, &position, &(position + forward), &up);
}

