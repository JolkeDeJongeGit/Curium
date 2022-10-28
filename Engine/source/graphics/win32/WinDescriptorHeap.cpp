#include "precomp.h"
#include "graphics/win32/WinDescriptorHeap.h"
#include <graphics/win32/WinDevice.h>

// Used from https://github.com/BredaUniversityGames/Y2022A-Y2-PR-Alpaca

DescriptorHeap::DescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t amountOfDescriptors, bool isShaderVisible)
{
	ComPtr<ID3D12Device2> device = Device::Get().GetDevice();

	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.Type = type;
	heapDesc.NumDescriptors = amountOfDescriptors;
	heapDesc.Flags = isShaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	ThrowIfFailed(device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_descriptorHeap)));
	m_descriptorSize = device->GetDescriptorHandleIncrementSize(type);

	m_maxDescriptorIndex = amountOfDescriptors - 1;
}

ComPtr<ID3D12DescriptorHeap>& DescriptorHeap::GetDescriptorHeap()
{
	return m_descriptorHeap;
}

CD3DX12_CPU_DESCRIPTOR_HANDLE DescriptorHeap::GetCPUHandleAt(uint32_t index)
{
	return CD3DX12_CPU_DESCRIPTOR_HANDLE(m_descriptorHeap->GetCPUDescriptorHandleForHeapStart(), index, m_descriptorSize);
}

CD3DX12_GPU_DESCRIPTOR_HANDLE DescriptorHeap::GetGPUHandleAt(uint32_t index)
{
	return CD3DX12_GPU_DESCRIPTOR_HANDLE(m_descriptorHeap->GetGPUDescriptorHandleForHeapStart(), index, m_descriptorSize);
}

uint32_t DescriptorHeap::GetDescriptorSize()
{
	return m_descriptorSize;
}

uint32_t DescriptorHeap::GetNextIndex()
{
	if (m_descriptorIndex > m_maxDescriptorIndex)
	{
		assert("Max amount of Descriptor Views within Descriptor Heap achieved!");
		return 0;
	}

	uint32_t index = m_descriptorIndex;
	m_descriptorIndex++;
	return index;
}
