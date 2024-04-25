#pragma once
#include "Systems/IExecute.h"


class LineColorDemo : public IExecute
{
public:
	virtual void Initialize() override; // �����ڿ�Ȱ
	virtual void Destroy() override; // �����Ҷ�
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

	Vertex vertices[2];
	ID3D11Buffer* vertexBuffer;
};
