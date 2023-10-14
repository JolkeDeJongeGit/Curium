#pragma once
class DescriptorHeap
{
public:
	DescriptorHeap() = default;
	DescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE inType, uint32_t inAmountOfDescriptors, bool inIsShaderVisible);

	ComPtr<ID3D12DescriptorHeap>& GetDescriptorHeap();
	CD3DX12_CPU_DESCRIPTOR_HANDLE GetCpuHandleAt(uint32_t inDex) const;
	CD3DX12_GPU_DESCRIPTOR_HANDLE GetGpuHandleAt(uint32_t inDex) const;

	uint32_t GetDescriptorSize() const;
	uint32_t GetNextIndex();
private:
	ComPtr<ID3D12DescriptorHeap> m_descriptorHeap;
	uint32_t m_descriptorSize = 0;
	uint32_t m_maxDescriptorIndex = 0;
	uint32_t m_descriptorIndex = 0;
};