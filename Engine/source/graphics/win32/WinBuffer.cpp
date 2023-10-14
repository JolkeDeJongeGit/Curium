#include "precomp.h"
#include "graphics/win32/WinDevice.h"
#include "graphics/win32/WinDescriptorHeap.h"
#include "graphics/win32/WinBuffer.h"

void Buffer::CreateConstantBuffer(uint32_t inSize)
{
	ComPtr<ID3D12Device2> device = WinUtil::GetDevice()->GetDevice();
	DescriptorHeap* cbv = WinUtil::GetDescriptorHeap(HeapType::CBV_SRV_UAV);

	m_size = inSize;
	int t = inSize / 256;
	int rem = inSize % 256;

	uint32_t bufferSize = t * 256;
	if (rem > 0)
	{
		bufferSize += 256;
	}

	D3D12_HEAP_PROPERTIES properties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	D3D12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize, D3D12_RESOURCE_FLAG_NONE);

	ThrowIfFailed(device->CreateCommittedResource(
		&properties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&m_buffer)));

	D3D12_CONSTANT_BUFFER_VIEW_DESC bufferDesc = {};
	bufferDesc.BufferLocation = m_buffer->GetGPUVirtualAddress();
	bufferDesc.SizeInBytes = bufferSize;

	m_index = cbv->GetNextIndex();

	device->CreateConstantBufferView(&bufferDesc, cbv->GetCpuHandleAt(m_index));
}

void Buffer::UpdateBuffer(void* inData)
{
	CD3DX12_RANGE range(0, 0);
	uint32_t* pBuffer;
	ThrowIfFailed(m_buffer->Map(0, &range, reinterpret_cast<void**>(&pBuffer)));
	memcpy(pBuffer, inData, m_size);
}

void Buffer::SetGraphicsRootConstantBufferView(const ComPtr<ID3D12GraphicsCommandList>& inCommandList, uint32_t inRootParamIndex)
{
	inCommandList->SetGraphicsRootConstantBufferView(inRootParamIndex, m_buffer->GetGPUVirtualAddress());
}
