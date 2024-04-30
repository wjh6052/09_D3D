#pragma once
#include "Systems/IExecute.h"


class CubeDemo : public IExecute
{
public:
	virtual void Initialize() override; // 생성자역활
	virtual void Destroy() override; // 종료할때
	virtual void Update() override;
	virtual void Render() override;
	virtual void PreRender() override {};
	virtual void PostRender() override {};
	virtual void ResizeScreen() override {};

private:
	struct Vertex
	{
		Vector3 Position;
	}; 

private:
	Shader* shader;

	UINT vertexCount;
	Vertex* vertices;
	ID3D11Buffer* vertexBuffer;

	UINT indexCount;
	UINT* indices;
	ID3D11Buffer* indexBuffer;

	float moveSpeed = 10.0f;
	float rotationSpeed = 10.0f;
	
	Vector3 position = Vector3(64, 0.5f, 64);
	Vector3 rotation = Vector3(0, 0, 0);
	Vector3 scale = Vector3(1, 1, 2);

	Matrix world;
};
