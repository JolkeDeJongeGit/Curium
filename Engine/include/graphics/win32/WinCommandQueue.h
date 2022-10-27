#pragma once
class CommandQueue
{
public:
	CommandQueue();
	~CommandQueue();

	void ExecuteCommandList();

	void UploadData(ComPtr<ID3D12Resource> resource, D3D12_SUBRESOURCE_DATA subresource);

	ComPtr<ID3D12CommandAllocator>& GetCommandAllocator();
	ComPtr<ID3D12GraphicsCommandList>& GetCommandList();
	ComPtr<ID3D12CommandQueue>& GetCommandQueue();
private:
	ComPtr<ID3D12CommandAllocator> m_commandAllocator;
	ComPtr<ID3D12GraphicsCommandList> m_commandList;
	ComPtr<ID3D12CommandQueue> m_commandQueue;
};