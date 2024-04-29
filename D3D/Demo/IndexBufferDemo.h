#pragma once
#include "Systems/IExecute.h"


class IndexBufferDemo : public IExecute
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

	Vertex vertices[4];
	ID3D11Buffer* vertexBuffer;

	UINT indices[6];
	ID3D11Buffer* indexBuffer;

	Matrix world;
};
