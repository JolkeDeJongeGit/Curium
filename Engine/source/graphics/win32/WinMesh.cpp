#include "precomp.h"

#include "common/AssetManager.h"
#include "graphics/Mesh.h"
#include "graphics/win32/WinCommandQueue.h"
#include "graphics/win32/WinDescriptorHeap.h"
#include "graphics/win32/WinDevice.h"

Texture::Texture(std::string inPath, std::vector<uint8_t> inData, glm::ivec2 inImageSize): m_descriptorIndex(0),
	m_path(inPath), m_imageSize(inImageSize)
{

	ComPtr<ID3D12Device2> device = WinUtil::GetDevice()->GetDevice();
	DescriptorHeap* srvHeap = WinUtil::GetDescriptorHeap(HeapType::CBV_SRV_UAV);
	CommandQueue* commands = WinUtil::GetCommandQueue();

	D3D12_HEAP_PROPERTIES properties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, inImageSize.x, inImageSize.y);

	ThrowIfFailed(device->CreateCommittedResource(
		&properties,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_COMMON,
		nullptr,
		IID_PPV_ARGS(&m_data)));

	D3D12_SUBRESOURCE_DATA subresource;
	subresource.pData = inData.data();
	subresource.RowPitch = inImageSize.x * sizeof(uint32_t);
	subresource.SlicePitch = inImageSize.x * inImageSize.y * sizeof(uint32_t);
	m_data->SetName(std::wstring(inPath.begin(), inPath.end()).c_str());
	commands->UploadData(m_data, subresource, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Texture2D.MipLevels = 1; // for now...

	m_descriptorIndex = srvHeap->GetNextIndex();
	device->CreateShaderResourceView(m_data.Get(), &srvDesc, srvHeap->GetCpuHandleAt(m_descriptorIndex));
}

glm::ivec2 Texture::GetSize() const
{ 
	return m_imageSize;
}

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
	int div = 8;
	for (int row = 0; row < div; row++)
	{
		for (int col = 0; col < div; col++)
		{
			int index = row * (div + 1) + col;			
			m_indexData.push_back(index);					
			m_indexData.push_back(index + 1);				
			m_indexData.push_back(index + (div + 1) + 1);	
			m_indexData.push_back(index + (div + 1));		
		}
	}

	glm::vec3 v0 = glm::vec3(-0.5f, 0.0f, -0.5f);
	glm::vec3 v1 = glm::vec3(-0.5f, 0.0f, 0.5f);
	glm::vec3 v2 = glm::vec3(0.5f, 0.0f, 0.5f);
	glm::vec3 v3 = glm::vec3(0.5f, 0.0f, -0.5f);

	glm::vec3 dir03 = (v3 - v0) / float(div);
	glm::vec3 dir12 = (v2 - v1) / float(div);
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> textureCoords;
	// dir2 and dir3
	for (float i = 0; i < div + 1; i++)
	{
		for (float j = 0; j < div + 1; j++)
		{
			glm::vec3 acrossj = ((v1 + i * dir12) - (v0 + i * dir03)) / float(div);
			glm::vec3 crntVec = v0 + i * dir03 + j * acrossj;
			// Position
			vertices.push_back(glm::vec3(crntVec.x, crntVec.y, crntVec.z));
			// Tex UV
			textureCoords.push_back(glm::vec2(float(j) / div, float(i) / div));
		}
	}

	for (size_t i = 0; i < vertices.size(); i++)
	{
		m_vertexData.push_back(VertexData(vertices[i], glm::vec3(0, 1, 0), textureCoords[i]));
	}

	//m_textureData.insert(std::pair("albedo", AssetManager::LoadTexture("assets/textures/cube_albedo.png")));
	m_textureData.insert(std::pair("heightmap", AssetManager::LoadTexture("assets/textures/heightmap.png")));
	//m_textureData.insert(std::pair("normal", AssetManager::LoadTexture("assets/textures/cube_normal.png")));
	//m_textureData.insert(std::pair("height", AssetManager::LoadTexture("assets/textures/cube_height.png")));
	//m_textureData.insert(std::pair("roughness", AssetManager::LoadTexture("assets/textures/cube_roughness.png")));
	//m_textureData.insert(std::pair("ao", AssetManager::LoadTexture("assets/textures/cube_ao.png")));
	
	CreateVertexBuffer();
	CreateIndexBuffer();
	CreateTexturesBuffer();
}

void Mesh::SetupSphere()
{
	const float radius = 1.f;
	const float sectorCount = 36.f;
	const float stackCount = 18.f;
	// clear memory of prev arrays
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> texCoords;

	float x, y, z, xy;                              // vertex position
	float nx, ny, nz, lengthInv = 1.0f / radius;    // vertex normal
	float s, t;                                     // vertex texCoord

	float sectorStep = 2.f * glm::pi<float>() / sectorCount;
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
		k1 = i * static_cast<int>(sectorCount + 1);     // beginning of current stack
		k2 = k1 + static_cast<int>(sectorCount) + 1;      // beginning of next stack

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

	m_textureData.insert(std::pair("albedo", AssetManager::LoadTexture("assets/textures/cube_albedo.png")));
	//m_textureData.insert(std::pair("normal", AssetManager::LoadTexture("assets/textures/cube_normal.png")));
	//m_textureData.insert(std::pair("height", AssetManager::LoadTexture("assets/textures/cube_height.png")));
	//m_textureData.insert(std::pair("roughness", AssetManager::LoadTexture("assets/textures/cube_roughness.png")));
	//m_textureData.insert(std::pair("ao", AssetManager::LoadTexture("assets/textures/cube_ao.png")));
	
	CreateVertexBuffer();
	CreateIndexBuffer();
	CreateTexturesBuffer();
}

void Mesh::Draw(const ComPtr<ID3D12GraphicsCommandList>& inCommandList) const
{
	if (m_textureData.find("heightmap") != m_textureData.end())
	{
		auto descriptorIndex = m_textureData.at("heightmap").GetDescriptorIndex();
		inCommandList->SetGraphicsRootDescriptorTable(2, WinUtil::GetDescriptorHeap(HeapType::CBV_SRV_UAV)->GetGpuHandleAt(descriptorIndex));
	}
	else
	{
		auto descriptorIndex = m_textureData.at("albedo").GetDescriptorIndex();
		inCommandList->SetGraphicsRootDescriptorTable(2, WinUtil::GetDescriptorHeap(HeapType::CBV_SRV_UAV)->GetGpuHandleAt(descriptorIndex));
	}

	inCommandList->IASetVertexBuffers(0, 1, &m_vertexView);
	inCommandList->IASetIndexBuffer(&m_indexView);
	inCommandList->DrawIndexedInstanced(static_cast<uint32_t>(m_indexData.size()), 1, 0, 0, 0);
}
