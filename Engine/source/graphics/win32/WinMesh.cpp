#include "precomp.h"
#include "graphics/Mesh.h"
#include "graphics/win32/WinDevice.h"

Texture::Texture(std::string inPath, std::vector<uint8_t> inData, glm::ivec2 inImageSize): m_descriptorIndex(0),
	m_imageSize()
{
}

glm::ivec2 Texture::GetSize() const
{ return m_imageSize; }

uint32_t Texture::GetDescriptorIndex() const
{
	return m_descriptorIndex;
}

VertexData::VertexData(const glm::vec3& inPosition, const glm::vec3& inNormal, const glm::vec2& inTexCoord)
{
	for (size_t i = 0; i < 3; i++)
	{
		const glm::length_t j = static_cast<glm::length_t>(i);
		Position[i] = inPosition[j];
		Normal[i] = inNormal[j];
	}

	for (size_t i = 0; i < 2; i++)
	{
		TexCoord[i] = inTexCoord[static_cast<glm::length_t>(i)];
	}
}


void Mesh::CreateVertexBuffer()
{
	const UINT bufferSize = static_cast<UINT>(m_vertexData.size() * sizeof(VertexData));

	const D3D12_HEAP_PROPERTIES properties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	const D3D12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize, D3D12_RESOURCE_FLAG_NONE);

	ThrowIfFailed( WinUtil::GetDevice()->GetDevice().Get()->CreateCommittedResource(
		&properties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&m_vertexBuffer)));

	UINT8* vertexData = nullptr;
	const D3D12_RANGE range{ 0, 0 };

	ThrowIfFailed(m_vertexBuffer->Map(0, &range, reinterpret_cast<void**>(&vertexData)));
	memcpy(vertexData, &m_vertexData[0], bufferSize);
	m_vertexBuffer->Unmap(0, nullptr);

	m_vertexView.BufferLocation = m_vertexBuffer->GetGPUVirtualAddress();
	m_vertexView.StrideInBytes = sizeof(VertexData);
	m_vertexView.SizeInBytes = bufferSize;
}

void Mesh::CreateIndexBuffer()
{
	
	const UINT bufferSize = static_cast<UINT>(m_indexData.size() * sizeof(uint16_t));

	const D3D12_HEAP_PROPERTIES properties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	const D3D12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize, D3D12_RESOURCE_FLAG_NONE);

	ThrowIfFailed(WinUtil::GetDevice()->GetDevice().Get()->CreateCommittedResource(
		&properties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&m_indexBuffer)));

	UINT8* indexData = nullptr;
	const D3D12_RANGE range{ 0, 0 };

	ThrowIfFailed(m_indexBuffer->Map(0, &range, reinterpret_cast<void**>(&indexData)));
	memcpy(indexData, &m_indexData[0], bufferSize);
	m_indexBuffer->Unmap(0, nullptr);

	m_indexView.BufferLocation = m_indexBuffer->GetGPUVirtualAddress();
	m_indexView.Format = DXGI_FORMAT_R16_UINT;
	m_indexView.SizeInBytes = bufferSize;
}

void Mesh::CreateTexturesBuffer()
{
}

void Mesh::SetupCube()
{
	m_indexData = 
	{
		0, 1, 2,
		0, 2, 3,
		6, 5, 4,
		6, 4, 7,
		4, 5, 1,
		4, 1, 0,
		3, 2, 6,
		3, 6, 7,
		1, 5, 6,
		1, 6, 2,
		4, 0, 3,
		4, 3, 7
	};

	const std::vector<glm::vec3> vertices =
	{
			glm::vec3(-1.0f, -1.0f, -1.0f),
			glm::vec3(-1.0f, 1.0f, -1.0f),
			glm::vec3(1.0f, 1.0f, -1.0f),
			glm::vec3(1.0f, -1.0f, -1.0f),
			glm::vec3(-1.0f, -1.0f, 1.0f),
			glm::vec3(-1.0f, 1.0f, 1.0f),
			glm::vec3(1.0f, 1.0f, 1.0f),
			glm::vec3(1.0f, -1.0f, 1.0f),
	};

	const std::vector<glm::vec2> textureCoords =
	{
			glm::vec2(0,0),
			glm::vec2(0,1),
			glm::vec2(1,1),
			glm::vec2(1,1),
			glm::vec2(1,1),
			glm::vec2(1,1),
			glm::vec2(1,1),
			glm::vec2(1,1)
	};

	const std::vector<glm::vec3> normals =
	{
			glm::vec3(0,0,0),
			glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3(1.0f, 1.0f, 0.0f),
			glm::vec3(1.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 0.0f, 1.0f),
			glm::vec3(1.0f, 1.0f, 1.0f),
			glm::vec3(1.0f, 0.0f, 1.0f),
			glm::vec3(1.0f, 0.0f, 1.0f),
	};
	
	for (size_t i = 0; i < normals.size(); i++)
	{
		m_vertexData.push_back(VertexData(vertices[i], normals[i], textureCoords[i]));
	}
	
	CreateVertexBuffer();
	CreateIndexBuffer();
	CreateTexturesBuffer();
}

void Mesh::SetupSphere()
{
	constexpr float radius = 1.f;
	constexpr float sectorCount = 36.f;
	constexpr float stackCount = 18.f;
	// clear memory of prev arrays
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> texCoords;

	float x, y, z, xy;                              // vertex position
	float nx, ny, nz, lengthInv = 1.0f / radius;    // vertex normal
	float s, t;                                     // vertex texCoord

	float sectorStep = 2 * glm::pi<float>() / sectorCount;
	float stackStep = glm::pi<float>() / stackCount;
	float sectorAngle, stackAngle;

	for(int i = 0; i <= stackCount; ++i)
	{
		stackAngle = glm::pi<float>() / 2 - i * stackStep;        // starting from pi/2 to -pi/2
		xy = radius * cosf(stackAngle);             // r * cos(u)
		z = radius * sinf(stackAngle);              // r * sin(u)

		// add (sectorCount+1) vertices per stack
		// first and last vertices have same position and normal, but different tex coords
		for(int j = 0; j <= sectorCount; ++j)
		{
			sectorAngle = j * sectorStep;           // starting from 0 to 2pi

			// vertex position (x, y, z)
			x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
			y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)
			vertices.emplace_back(x, y, z);

			// normalized vertex normal (nx, ny, nz)
			nx = x * lengthInv;
			ny = y * lengthInv;
			nz = z * lengthInv;
			normals.emplace_back(nx, ny, nz);

			// vertex tex coord (s, t) range between [0, 1]
			s = static_cast<float>(j) / sectorCount;
			t = static_cast<float>(i) / stackCount;
			texCoords.emplace_back(s,t);
		}
	}

	int k1, k2;
	for(int i = 0; i < stackCount; ++i)
	{
		k1 = i * (sectorCount + 1);     // beginning of current stack
		k2 = k1 + sectorCount + 1;      // beginning of next stack

		for(int j = 0; j < sectorCount; ++j, ++k1, ++k2)
		{
			// 2 triangles per sector excluding first and last stacks
			// k1 => k2 => k1+1
			if(i != 0)
			{
				m_indexData.push_back(k1);
				m_indexData.push_back(k2);
				m_indexData.push_back(k1 + 1);
			}

			// k1+1 => k2 => k2+1
			if(i != (stackCount-1))
			{
				m_indexData.push_back(k1 + 1);
				m_indexData.push_back(k2);
				m_indexData.push_back(k2 + 1);
			}
		}
	}

	for (size_t i = 0; i < normals.size(); i++)
	{
		m_vertexData.push_back(VertexData(vertices[i], normals[i], texCoords[i]));
	}
	
	CreateVertexBuffer();
	CreateIndexBuffer();
	CreateTexturesBuffer();
}

void Mesh::Draw(const ComPtr<ID3D12GraphicsCommandList>& inCommandList) const
{
	inCommandList->IASetVertexBuffers(0, 1, &m_vertexView);
	inCommandList->IASetIndexBuffer(&m_indexView);
	inCommandList->DrawIndexedInstanced(static_cast<uint32_t>(m_indexData.size()), 1, 0, 0, 0);
}
