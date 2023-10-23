#pragma once

/// <summary>
/// This is stores memory for the cpu and gpu
/// </summary>
class Buffer
{
public:
	/// <summary>
	/// Creates free space on the gpu and defines this as a constant buffer
	/// </summary>
	/// <param name="inSize">the byte size</param>
	void CreateConstantBuffer(uint32_t inSize);

	/// <summary>
	/// Here you update the data in the buffer
	/// </summary>
	/// <param name="inData"></param>
	void UpdateBuffer(void* inData);

	/// <summary>
	/// Here you link the buffer to gpu. 
	/// </summary>
	/// <param name="inCommandList"></param>
	/// <param name="inRootParamIndex"></param>
	void SetGraphicsRootConstantBufferView(const ComPtr<ID3D12GraphicsCommandList>& inCommandList, uint32_t inRootParamIndex);

	inline ID3D12Resource* GetBuffer() const { return m_buffer.Get(); }
	inline uint32_t const GetBufferSize() { return m_size; }
	inline uint32_t const GetDescriptorIndex() { return m_index; }
private: 
	ComPtr<ID3D12Resource> m_buffer;
	uint32_t* m_pBuffer;
	uint32_t m_size;
	uint32_t m_index;
};