#pragma once

class Buffer
{
public:
	void CreateConstantBuffer(uint32_t inSize);
	void UpdateBuffer(void* inData);

	void SetGraphicsRootConstantBufferView(const ComPtr<ID3D12GraphicsCommandList>& inCommandList, uint32_t inRootParamIndex);

	inline ID3D12Resource* GetBuffer() const { return m_buffer.Get(); }
	inline uint32_t const GetBufferSize() { return m_size; }
	inline uint32_t const GetDescriptorIndex() { return m_index; }
private: 
	ComPtr<ID3D12Resource> m_buffer;
	uint32_t m_size;
	uint32_t m_index;
};