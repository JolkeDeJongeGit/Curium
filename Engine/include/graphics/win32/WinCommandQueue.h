#pragma once
#include "WinCommandList.h"
class CommandQueue
{
public:
	CommandQueue();
	CommandQueue(D3D12_COMMAND_LIST_TYPE _type);
	~CommandQueue();

	static CommandQueue& Get()
	{
		static CommandQueue instance;
		return instance;
	}

	void ExecuteCommandList();

	void UploadData(ComPtr<ID3D12Resource> _resource, D3D12_SUBRESOURCE_DATA _subresource);

	CommandList& GetCommandList();
	ComPtr<ID3D12CommandQueue>& GetCommandQueue();
private:
	CommandList m_commandList;
	ComPtr<ID3D12CommandQueue> m_commandQueue;
};