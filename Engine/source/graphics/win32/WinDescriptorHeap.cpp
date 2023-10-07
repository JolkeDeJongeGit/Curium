#include "precomp.h"
#include "graphics/win32/WinDescriptorHeap.h"
#include <graphics/win32/WinDevice.h>

#include "graphics/win32/WinUtil.h"

// Used from https://github.com/BredaUniversityGames/Y2022A-Y2-PR-Alpaca

DescriptorHeap::DescriptorHeap(const D3D12_DESCRIPTOR_HEAP_TYPE inType, const uint32_t inAmountOfDescriptors, const bool inIsShaderVisible)
{
	const ComPtr<ID3D12Device2> device = WinUtil::GetDevice()->GetDevice();

	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.Type = inType;
	heapDesc.NumDescriptors = inAmountOfDescriptors;
	heapDesc.Flags = inIsShaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	ThrowIfFailed(device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_descriptorHeap)));
	m_descriptorSize = device->GetDescriptorHandleIncrementSize(inType);

	m_maxDescriptorIndex = inAmountOfDescriptors - 1;
}

ComPtr<ID3D12DescriptorHeap>& DescriptorHeap::GetDescriptorHeap()
{
	return m_descriptorHeap;
}

CD3DX12_CPU_DESCRIPTOR_HANDLE DescriptorHeap::GetCpuHandleAt(const uint32_t inDex) const
{
	return CD3DX12_CPU_DESCRIPTOR_HANDLE(m_descriptorHeap->GetCPUDescriptorHandleForHeapStart(), inDex, m_descriptorSize);
}

CD3DX12_GPU_DESCRIPTOR_HANDLE DescriptorHeap::GetGpuHandleAt(const uint32_t inDex) const
{
	auto value = m_descriptorHeap->GetGPUDescriptorHandleForHeapStart();
	auto value1 = CD3DX12_GPU_DESCRIPTOR_HANDLE(value, inDex, m_descriptorSize);
	return value1;
}

uint32_t DescriptorHeap::GetDescriptorSize() const
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

	const uint32_t index = m_descriptorIndex;
	m_descriptorIndex++;
	return index;
}
