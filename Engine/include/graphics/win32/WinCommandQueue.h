#pragma once
#include "WinCommandList.h"
#include <optional>
class CommandQueue
{
public:
	CommandQueue();
	CommandQueue(D3D12_COMMAND_LIST_TYPE inType);
	~CommandQueue();
	
	void OpenCommandList();
	void CloseCommandList();

	void UploadData(ComPtr<ID3D12Resource> inResource, D3D12_SUBRESOURCE_DATA inSubresource, D3D12_RESOURCE_STATES inInitialState, std::optional<D3D12_RESOURCE_STATES> inFinaState = {});
	
	uint64_t Signal();
	void WaitForFence(uint64_t inFenceValue);
	bool IsFenceComplete(uint64_t fenceValue);
	
	void ExecuteCommandList();
	CommandList& GetCommandList();
	ComPtr<ID3D12CommandQueue>& GetCommandQueue();
private:
	ComPtr<ID3D12Fence> m_fence = nullptr;
	HANDLE m_fenceEvent = nullptr;
	uint64_t m_fenceValue = 0;

	CommandList m_commandList;
	ComPtr<ID3D12CommandQueue> m_commandQueue;
	std::vector<ComPtr<ID3D12Resource>> m_intermediateResources;
};