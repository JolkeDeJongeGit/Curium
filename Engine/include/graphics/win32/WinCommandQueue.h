#pragma once
#include "WinCommandList.h"
class CommandQueue
{
public:
	CommandQueue();
	CommandQueue(D3D12_COMMAND_LIST_TYPE inType);
	~CommandQueue();
	
	void ExecuteCommandList();

	void UploadData(ComPtr<ID3D12Resource> inResource, D3D12_SUBRESOURCE_DATA inSubresource);

	CommandList& GetCommandList();
	ComPtr<ID3D12CommandQueue>& GetCommandQueue();
private:
	CommandList m_commandList;
	ComPtr<ID3D12CommandQueue> m_commandQueue;
};