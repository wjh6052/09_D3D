#include "Framework.h"
#include "StaticMeshRanderer.h"


StaticMeshRanderer::StaticMeshRanderer(Shader* shader)
	: shader(shader), bCreatedShader(false)
{
	Initialize();
}

StaticMeshRanderer::StaticMeshRanderer(wstring shaderFile)
	: bCreatedShader(true)
{
	shader = new Shader(shaderFile);
		
	Initialize();
}

void StaticMeshRanderer::Initialize()
{
	transform = new Transform(shader);
	perFrame = new PerFrame(shader);
}

StaticMeshRanderer::~StaticMeshRanderer()
{
	SafeDelete(transform);
	SafeDelete(perFrame);

	SafeDelete(vertexBuffer);
	SafeDelete(indexBuffer);

	if (bCreatedShader)
		SafeDelete(shader)
}

void StaticMeshRanderer::Update()
{
	transform->Update();
	perFrame->Update();
}

void StaticMeshRanderer::Render()
{
	if (vertexBuffer != nullptr)
	{
		vertexBuffer->IASet();

		if(indexBuffer != nullptr)
			indexBuffer->IASet();
	}

	D3D::GetDC()->IASetPrimitiveTopology(topology);


	transform->Render();
	perFrame->Render();
}

