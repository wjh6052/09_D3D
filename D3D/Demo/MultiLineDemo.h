#pragma once
#include "Systems/IExecute.h"


class MultiLineDemo : public IExecute
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
		Color Color;
	}; 

private:
	Shader* shader;

	Vertex vertices[6];
	ID3D11Buffer* vertexBuffer;
};
