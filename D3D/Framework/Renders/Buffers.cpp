#include "Framework.h"
#include "Buffers.h"



//-----------------------------------------------------------------------------
//VertexBuffer
//-----------------------------------------------------------------------------

VertexBuffer::VertexBuffer(void* data, UINT count, UINT stride, UINT slot, bool bCpuWrite, bool bGpuWrite)
	:data(data), count(count), stride(stride), slot(slot), bCpuWrite(bCpuWrite), bGpuWrite(bGpuWrite)
{
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.ByteWidth = stride * count;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	if (bCpuWrite == false && bGpuWrite == false)
	{
		desc.Usage = D3D11_USAGE_IMMUTABLE;
	}
	else if (bCpuWrite == true && bGpuWrite == false)
	{
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	}
	else if (bCpuWrite == true && bGpuWrite == false)
	{
		desc.Usage = D3D11_USAGE_DEFAULT;
	}
	else if (bCpuWrite == true && bGpuWrite == true)
	{
		desc.Usage = D3D11_USAGE_STAGING;
		desc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	}
	//cpu write -> Map

	D3D11_SUBRESOURCE_DATA subResource = { 0 };
	subResource.pSysMem = data;

	Check(D3D::GetDevice()->CreateBuffer(&desc, &subResource, &buffer));
}

VertexBuffer::~VertexBuffer()
{
	SafeRelease(buffer);
}

void VertexBuffer::IASet()
{
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(slot, 1, &buffer, &stride, &offset);
}



//-----------------------------------------------------------------------------
//IndexBuffer
//-----------------------------------------------------------------------------

IndexBuffer::IndexBuffer(void* data, UINT count)
	:data(data), count(count)
{
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.ByteWidth = sizeof(UINT) * count;
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	desc.Usage = D3D11_USAGE_IMMUTABLE;


	D3D11_SUBRESOURCE_DATA subResource = { 0 };
	subResource.pSysMem = data;

	Check(D3D::GetDevice()->CreateBuffer(&desc, &subResource, &buffer));
}

IndexBuffer::~IndexBuffer()
{
	SafeRelease(buffer);
}


void IndexBuffer::IASet()
{
	D3D::GetDC()->IASetIndexBuffer(buffer, DXGI_FORMAT_R32_UINT, 0);
}


//-----------------------------------------------------------------------------
//ConstantBuffer
//-----------------------------------------------------------------------------

ConstantBuffer::ConstantBuffer(void* data, UINT dataSize)
	:data(data), dataSize(dataSize)
{
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.ByteWidth = dataSize;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	Check(D3D::GetDevice()->CreateBuffer(&desc, nullptr, &buffer));
}

ConstantBuffer::~ConstantBuffer()
{
	SafeRelease(buffer);
}

void ConstantBuffer::Map()
{
	D3D11_MAPPED_SUBRESOURCE subResource;

	D3D::GetDC()->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
	memcpy(subResource.pData, data, dataSize);
	D3D::GetDC()->Unmap(buffer, 0);
}




//-----------------------------------------------------------------------------
//CsResource(Super)
//
CsResource::CsResource()
{
	 
}

CsResource::~CsResource()
{
	SafeRelease(input);
	SafeRelease(srv);
	SafeRelease(output);
	SafeRelease(uav);
}

void CsResource::CreateBuffer()
{
	CreateInput();
	CreateSRV();

	CreateOutput();
	CreateUAV();
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
//RawBuffer(Super)
//

RawBuffer::RawBuffer(void* data, UINT inputByte, UINT outputByte)
	: inputData(data)
	, inputByte(inputByte)
	, outputByte(outputByte)
{
	CreateBuffer();
}

RawBuffer::~RawBuffer()
{
}

void RawBuffer::CreateInput()
{
	if (inputByte < 1) return;

	ID3D11Buffer* buffer = nullptr;

	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));

	desc.ByteWidth = inputByte;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	D3D11_SUBRESOURCE_DATA subResource = { 0 };
	subResource.pSysMem = inputData;

	Check(D3D::GetDevice()->CreateBuffer(&desc, inputData != nullptr ? &subResource : nullptr, &buffer));

	input = buffer;
}

void RawBuffer::CreateSRV()
{
	if (inputByte < 1) return;

	ID3D11Buffer* buffer = (ID3D11Buffer*)input;

	D3D11_BUFFER_DESC desc;
	buffer->GetDesc(&desc);


	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	srvDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	srvDesc.BufferEx.Flags = D3D11_BUFFEREX_SRV_FLAG_RAW;
	srvDesc.BufferEx.NumElements = desc.ByteWidth / 4;

	Check(D3D::GetDevice()->CreateShaderResourceView(buffer, &srvDesc, &srv));
}

void RawBuffer::CreateOutput()
{
	ID3D11Buffer* buffer = nullptr;

	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));

	desc.ByteWidth = outputByte;
	desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

	Check(D3D::GetDevice()->CreateBuffer(&desc, nullptr, &buffer));

	output = buffer;
}

void RawBuffer::CreateUAV()
{
	ID3D11Buffer* buffer = (ID3D11Buffer*)output;

	D3D11_BUFFER_DESC desc;
	buffer->GetDesc(&desc);

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	ZeroMemory(&uavDesc, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));
	uavDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW;
	uavDesc.Buffer.NumElements = desc.ByteWidth / 4;

	Check(D3D::GetDevice()->CreateUnorderedAccessView(buffer, &uavDesc, &uav));
}

void RawBuffer::CopyToInput(void* data)
{
	D3D11_MAPPED_SUBRESOURCE subResource;
	D3D::GetDC()->Map(input, 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
	{
		memcpy(subResource.pData, data, inputByte);
	}
	D3D::GetDC()->Unmap(input, 0);
}

void RawBuffer::CopyFromOutput(void* data)
{
	D3D11_MAPPED_SUBRESOURCE subResource;
	D3D::GetDC()->Map(output, 0, D3D11_MAP_READ, 0, &subResource);
	{
		memcpy(data, subResource.pData, outputByte);
	}
	D3D::GetDC()->Unmap(output, 0);
}
}
//-----------------------------------------------------------------------------
