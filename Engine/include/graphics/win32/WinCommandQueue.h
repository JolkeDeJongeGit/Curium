#pragma once
#include "WinCommandList.h"
class CommandQueue
{
public:
	CommandQueue();
	CommandQueue(D3D12_COMMAND_LIST_TYPE inType);
	~CommandQueue();
	
	void OpenCommandList();
	void CloseCommandList();

	void UploadData(ComPtr<ID3D12Resource> inResource, D3D12_SUBRESOURCE_DATA inSubresource, D3D12_RESOURCE_STATES inInitialState);

	void ExecuteCommandList();
	CommandList& GetCommandList();
	ComPtr<ID3D12CommandQueue>& GetCommandQueue();
private:
	CommandList m_commandList;
	ComPtr<ID3D12CommandQueue> m_commandQueue;
	std::vector<ComPtr<ID3D12Resource>> m_intermediateResources;
};