#include "precomp.h"
#include "graphics/Mesh.h"
#include "graphics/win32/WinUtil.h"
#include "graphics/win32/WinDevice.h"
#include "graphics/win32/WinCommandQueue.h"
#include "graphics/win32/WinCommandList.h"

Texture::Texture(std::string path, std::vector<uint8_t> data, glm::ivec2 imageSize)
{
}

Texture::~Texture()
{
}


VertexData::VertexData(const glm::vec3& position, const glm::vec3& normal, const glm::vec2& texCoord)
{
	for (size_t i = 0; i < 3; i++)
	{
		glm::length_t j = static_cast<glm::length_t>(i);
		Position[i] = position[j];
		Normal[i] = normal[j];
	}

	for (size_t i = 0; i < 2; i++)
	{
		TexCoord[i] = texCoord[static_cast<glm::length_t>(i)];
	}
}


void Mesh::CreateVertexBuffer()
{
	const UINT bufferSize = static_cast<UINT>(m_vertexData.size() * sizeof(VertexData));

	D3D12_HEAP_PROPERTIES properties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	D3D12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize, D3D12_RESOURCE_FLAG_NONE);

	ThrowIfFailed(Device::Get().GetDevice().Get()->CreateCommittedResource(
		&properties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&m_vertexBuffer.m_bufferResource)));

	UINT8* vertexData = nullptr;
	D3D12_RANGE range{ 0, 0 };

	ThrowIfFailed(m_vertexBuffer.m_bufferResource->Map(0, &range, reinterpret_cast<void**>(&vertexData)));
	memcpy(vertexData, &m_vertexData[0], bufferSize);
	m_vertexBuffer.m_bufferResource->Unmap(0, nullptr);

	m_vertexBuffer.m_vertexView.BufferLocation = m_vertexBuffer.m_bufferResource->GetGPUVirtualAddress();
	m_vertexBuffer.m_vertexView.StrideInBytes = sizeof(VertexData);
	m_vertexBuffer.m_vertexView.SizeInBytes = bufferSize;
}

void Mesh::CreateIndexBuffer()
{
	const UINT bufferSize = static_cast<UINT>(m_indexData.size() * sizeof(uint16_t));

	D3D12_HEAP_PROPERTIES properties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	D3D12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize, D3D12_RESOURCE_FLAG_NONE);

	ThrowIfFailed(Device::Get().GetDevice().Get()->CreateCommittedResource(
		&properties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&m_indexBuffer.m_bufferResource)));

	UINT8* indexData = nullptr;
	D3D12_RANGE range{ 0, 0 };

	ThrowIfFailed(m_indexBuffer.m_bufferResource->Map(0, &range, reinterpret_cast<void**>(&indexData)));
	memcpy(indexData, &m_indexData[0], bufferSize);
	m_indexBuffer.m_bufferResource->Unmap(0, nullptr);

	m_vertexBuffer.m_indexView.BufferLocation = m_indexBuffer.m_bufferResource->GetGPUVirtualAddress();
	m_vertexBuffer.m_indexView.Format = DXGI_FORMAT_R16_UINT;
	m_vertexBuffer.m_indexView.SizeInBytes = bufferSize;
}

void Mesh::CreateTexturesBuffer()
{
}

void Mesh::SetupCube()
{
	m_indexData = 
	{
		0,1,3,
		3,1,2,
		4,5,7,
		7,5,6,
		8,9,11,
		11,9,10,
		12,13,15,
		15,13,14,
		16,17,19,
		19,17,18,
		20,21,23,
		23,21,22
	};

	const std::vector<glm::vec3> vertices =
	{
				glm::vec3( - 0.5f,0.5f,-0.5f),
				glm::vec3(-0.5f,-0.5f,-0.5f),
				glm::vec3(0.5f,-0.5f,-0.5f),
				glm::vec3(0.5f,0.5f,-0.5f),

				glm::vec3(-0.5f,0.5f,0.5f),
				glm::vec3(-0.5f,-0.5f,0.5f),
				glm::vec3(0.5f,-0.5f,0.5f),
				glm::vec3(0.5f,0.5f,0.5f),

				glm::vec3(0.5f,0.5f,-0.5f),
				glm::vec3(0.5f,-0.5f,-0.5f),
				glm::vec3(0.5f,-0.5f,0.5f),
				glm::vec3(0.5f,0.5f,0.5f),

				glm::vec3(-0.5f,0.5f,-0.5f),
				glm::vec3(-0.5f,-0.5f,-0.5f),
				glm::vec3(-0.5f,-0.5f,0.5f),
				glm::vec3(-0.5f,0.5f,0.5f),

				glm::vec3(-0.5f,0.5f,0.5f),
				glm::vec3(-0.5f,0.5f,-0.5f),
				glm::vec3(0.5f,0.5f,-0.5f),
				glm::vec3(0.5f,0.5f,0.5f),

				glm::vec3(-0.5f,-0.5f,0.5f),
				glm::vec3(-0.5f,-0.5f,-0.5f),
				glm::vec3(0.5f,-0.5f,-0.5f),
				glm::vec3(0.5f,-0.5f,0.5f)
	};

	const std::vector<glm::vec2> textureCoords =
	{
			glm::vec2(0,0),
			glm::vec2(0,1),
			glm::vec2(1,1),
			glm::vec2(1,0),
			glm::vec2(0,0),
			glm::vec2(0,1),
			glm::vec2(1,1),
			glm::vec2(1,0),
			glm::vec2(0,0),
			glm::vec2(0,1),
			glm::vec2(1,1),
			glm::vec2(1,0),
			glm::vec2(0,0),
			glm::vec2(0,1),
			glm::vec2(1,1),
			glm::vec2(1,0),
			glm::vec2(0,0),
			glm::vec2(0,1),
			glm::vec2(1,1),
			glm::vec2(1,0),
			glm::vec2(0,0),
			glm::vec2(0,1),
			glm::vec2(1,1),
			glm::vec2(1,0)
	};

	const std::vector<glm::vec3> normals =
	{
			glm::vec3(0,0,0),
			glm::vec3(0,0,0),
			glm::vec3(0,0,0),
			glm::vec3(0,0,0),

			glm::vec3(0,0,0),
			glm::vec3(0,0,0),
			glm::vec3(0,0,0),
			glm::vec3(0,0,0),

			glm::vec3(0,0,0),
			glm::vec3(0,0,0),
			glm::vec3(0,0,0),
			glm::vec3(0,0,0),

			glm::vec3(0,0,0),
			glm::vec3(0,0,0),
			glm::vec3(0,0,0),
			glm::vec3(0,0,0),

			glm::vec3(0,0,0),
			glm::vec3(0,0,0),
			glm::vec3(0,0,0),
			glm::vec3(0,0,0),

			glm::vec3(0,0,0),
			glm::vec3(0,0,0),
			glm::vec3(0,0,0),
			glm::vec3(0,0,0),
	};
	
	for (size_t i = 0; i < normals.size(); i++)
	{
		m_vertexData.push_back(VertexData(vertices[i], normals[i], textureCoords[i]));
	}
}

void Mesh::Draw(glm::mat4 _matrix, ComPtr<ID3D12GraphicsCommandList>& commandList)
{
	commandList->IASetVertexBuffers(0, 1, &m_vertexBuffer.m_vertexView);
	commandList->IASetIndexBuffer(&m_indexBuffer.m_indexView);
	commandList->DrawIndexedInstanced(static_cast<uint32_t>(m_indexData.size()), 1, 0, 0, 0);
}
