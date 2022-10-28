#pragma once
class DescriptorHeap
{
public:
	DescriptorHeap() = default;
	DescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t amountOfDescriptors, bool isShaderVisible);

	ComPtr<ID3D12DescriptorHeap>& GetDescriptorHeap();
	CD3DX12_CPU_DESCRIPTOR_HANDLE GetCPUHandleAt(uint32_t index);
	CD3DX12_GPU_DESCRIPTOR_HANDLE GetGPUHandleAt(uint32_t index);

	uint32_t GetDescriptorSize();
	uint32_t GetNextIndex();
private:
	ComPtr<ID3D12DescriptorHeap> m_descriptorHeap;
	uint32_t m_descriptorSize = 0;
	uint32_t m_maxDescriptorIndex = 0;
	uint32_t m_descriptorIndex = 0;
};