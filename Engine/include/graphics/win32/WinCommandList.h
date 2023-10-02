#pragma once
class CommandList
{
public:
	CommandList();
	CommandList(D3D12_COMMAND_LIST_TYPE inType);
	~CommandList();
	ComPtr<ID3D12CommandAllocator>& GetAllocater();
	ComPtr<ID3D12GraphicsCommandList>& GetList();
private:
	ComPtr<ID3D12CommandAllocator> m_commandAllocator;
	ComPtr<ID3D12GraphicsCommandList> m_commandList;
};