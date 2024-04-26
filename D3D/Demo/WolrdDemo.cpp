#include "stdafx.h"
#include "WolrdDemo.h"



void WolrdDemo::Initialize()
{
	shader = new Shader(L"05_World.fxo");

	D3DXMatrixIdentity(&world);
	

	//Polygon1
	vertices[0].Position = Vector3(-0.5f, -0.5f, 0.0f);
	vertices[1].Position = Vector3(-0.5f, +0.5f, 0.0f);
	vertices[2].Position = Vector3(+0.5f, -0.5f, 0.0f);
	vertices[3].Position = Vector3(+0.5f, -0.5f, 0.0f);
	vertices[4].Position = Vector3(-0.5f, +0.5f, 0.0f);
	vertices[5].Position = Vector3(+0.5f, +0.5f, 0.0f);
												   


	//Create VertexBuffer
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		desc.ByteWidth = sizeof(Vertex) * 6;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;



		D3D11_SUBRESOURCE_DATA subResource = { 0 };
		subResource.pSysMem = vertices;

		Check(D3D::GetDevice()->CreateBuffer(&desc, &subResource, &vertexBuffer));
	}
	


}


void WolrdDemo::Destroy()
{
	SafeDelete(shader);
	SafeRelease(vertexBuffer);

}


void WolrdDemo::Update()
{
	//Position
	if(Keyboard::Get()->Press('D'))
		world._41 += 2.0f * Time::Delta();

	if (Keyboard::Get()->Press('A'))
		world._41 -= 2.0f * Time::Delta();

	if (Keyboard::Get()->Press('W'))
		world._42 += 2.0f * Time::Delta();

	if (Keyboard::Get()->Press('S'))
		world._42 -= 2.0f * Time::Delta();

	//Todo. D3DXMatrixTranslation();

	//Scale
	if (Keyboard::Get()->Press(VK_RIGHT))
		world._11 += 2.0f * Time::Delta();

	if (Keyboard::Get()->Press(VK_LEFT))
		world._11 -= 2.0f * Time::Delta();

	if (Keyboard::Get()->Press(VK_UP))
		world._22 += 2.0f * Time::Delta();

	if (Keyboard::Get()->Press(VK_DOWN))
		world._22 -= 2.0f * Time::Delta();

}


void WolrdDemo::Render()
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;


	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);


	shader->AsMatrix("World")->SetMatrix(world);
	shader->AsMatrix("View")->SetMatrix(Context::Get()->View());
	shader->AsMatrix("Projection")->SetMatrix(Context::Get()->Projection());




	static bool bWireFrame;
	ImGui::Checkbox("WireFrame", &bWireFrame);

	shader->Draw(0, bWireFrame ? 1 : 0, 6);
	



}

