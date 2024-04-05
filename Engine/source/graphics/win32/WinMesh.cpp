#include "precomp.h"

#include "common/AssetManager.h"
#include "graphics/Mesh.h"
#include "graphics/win32/WinCommandQueue.h"
#include "graphics/win32/WinDescriptorHeap.h"
#include "graphics/win32/WinDevice.h"
#include "graphics/win32/WinCommandList.h"

// @TODO:: Make seperate file
Texture::Texture(std::string inPath, std::vector<uint8_t> inData, glm::ivec2 inImageSize): m_descriptorIndex(0),
	m_imageSize(inImageSize)
{
	ComPtr<ID3D12Device2> device = WinUtil::GetDevice()->GetDevice();
	DescriptorHeap* srvHeap = WinUtil::GetDescriptorHeap(HeapType::CBV_SRV_UAV);
	CommandQueue* commands = WinUtil::GetCommandQueue();

	D3D12_HEAP_PROPERTIES properties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, inImageSize.x, inImageSize.y);
	desc.MipLevels = 1;

	ThrowIfFailed(device->CreateCommittedResource(
		&properties,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_COMMON,
		nullptr,
		IID_PPV_ARGS(&m_data)));

	// -- Upload subresource 
	D3D12_SUBRESOURCE_DATA subresource;
	subresource.pData = inData.data();
	subresource.RowPitch = inImageSize.x * sizeof(uint32_t);
	subresource.SlicePitch = inImageSize.x * inImageSize.y * sizeof(uint32_t);
	commands->UploadData(m_data, subresource, D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	// -- Set Debug Name
	m_data->SetName(std::wstring(inPath.begin(), inPath.end()).c_str());

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Texture2D.MipLevels = 1; // for now...

	m_descriptorIndex = srvHeap->GetNextIndex();
	device->CreateShaderResourceView(m_data.Get(), &srvDesc, srvHeap->GetCpuHandleAt(m_descriptorIndex));
	m_currentState = D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
}

void Texture::SetState(D3D12_RESOURCE_STATES inSetState)
{
	if (m_currentState == inSetState)
		return;
	
	m_currentState = inSetState;

	// -- Switch Resource Barrier
	auto& commands = WinUtil::GetCommandQueue()->GetCommandList().GetList();
	CD3DX12_RESOURCE_BARRIER changeBarrier = CD3DX12_RESOURCE_BARRIER::Transition(m_data.Get(), m_currentState, inSetState);
	commands->ResourceBarrier(1, &changeBarrier);
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

void Mesh::Shutdown()
{
	m_vertexBuffer.Reset();
	m_indexBuffer.Reset();
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

void Mesh::Draw(const ComPtr<ID3D12GraphicsCommandList>& inCommandList) const
{
	// -- Send over the Views
	inCommandList->IASetVertexBuffers(0, 1, &m_vertexView);
	inCommandList->IASetIndexBuffer(&m_indexView);

	// -- Draw based on Indices
	inCommandList->DrawIndexedInstanced(static_cast<uint32_t>(m_indexData.size()), 1, 0, 0, 0);
}
