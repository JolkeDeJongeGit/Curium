#include <precomp.h>
#include "graphics/win32/WinCommandQueue.h"
#include <graphics/win32/WinDevice.h>
#include <graphics/win32/WinSwapchain.h>

CommandQueue::CommandQueue()
{
	const auto device =  WinUtil::GetDevice()->GetDevice();
	D3D12_COMMAND_QUEUE_DESC description = {};
	description.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	description.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	ThrowIfFailed(device->CreateCommandQueue(&description, IID_PPV_ARGS(&m_commandQueue)));

	ThrowIfFailed(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));

	ThrowIfFailed(m_commandList.GetList()->Close());
}

CommandQueue::CommandQueue(D3D12_COMMAND_LIST_TYPE inType)
{
}

CommandQueue::~CommandQueue()
{
}

void CommandQueue::OpenCommandList()
{
	auto& list = m_commandList.GetList();
	auto& alloc = m_commandList.GetAllocater();

	ThrowIfFailed(alloc->Reset());
	ThrowIfFailed(list->Reset(alloc.Get(), nullptr));
}

void CommandQueue::CloseCommandList()
{
	auto& list = m_commandList.GetList();

	list->Close();

	ExecuteCommandList();

	Signal();
	WaitForFence(m_fenceValue);

	m_intermediateResources.clear();
}
 
void CommandQueue::ExecuteCommandList()
{
	ID3D12CommandList* const commandLists[] = { m_commandList.GetList().Get()};
	m_commandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);
}

void CommandQueue::UploadData(ComPtr<ID3D12Resource> inResource, D3D12_SUBRESOURCE_DATA inSubresource, D3D12_RESOURCE_STATES inInitialState, std::optional<D3D12_RESOURCE_STATES> inFinaState)
{
	auto& list = m_commandList.GetList();
	ComPtr<ID3D12Device2> device = WinUtil::GetDevice()->GetDevice();
	CD3DX12_RESOURCE_BARRIER copyBarrier = CD3DX12_RESOURCE_BARRIER::Transition(inResource.Get(), inInitialState, D3D12_RESOURCE_STATE_COPY_DEST);
	CD3DX12_RESOURCE_BARRIER finalBarrier = CD3DX12_RESOURCE_BARRIER::Transition(inResource.Get(), D3D12_RESOURCE_STATE_COPY_DEST, inFinaState.value_or(inInitialState));
	list->ResourceBarrier(1, &copyBarrier);

	const UINT64 size = GetRequiredIntermediateSize(inResource.Get(), 0, 1);
	ComPtr<ID3D12Resource> intermediate;

	const D3D12_HEAP_PROPERTIES properties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	const D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(size);

	ThrowIfFailed(device->CreateCommittedResource(
		&properties,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&intermediate)
	));

	UpdateSubresources(list.Get(), inResource.Get(), intermediate.Get(), 0, 0, 1, &inSubresource);
	list->ResourceBarrier(1, &finalBarrier);
	m_intermediateResources.push_back(intermediate);
}

uint64_t CommandQueue::Signal()
{
	ThrowIfFailed(m_commandQueue->Signal(m_fence.Get(), ++m_fenceValue));
	return m_fenceValue;
}

void CommandQueue::WaitForFence(uint64_t inFenceValue)
{
	if (!IsFenceComplete(m_fenceValue))
	{
		auto event = ::CreateEvent(NULL, FALSE, FALSE, NULL);
		if (event)
		{
			// Is this function thread safe?
			m_fence->SetEventOnCompletion(m_fenceValue, event);
			::WaitForSingleObject(event, DWORD_MAX);

			::CloseHandle(event);
		}
	}
}

bool CommandQueue::IsFenceComplete(uint64_t fenceValue)
{
	return m_fence->GetCompletedValue() >= fenceValue;
}


CommandList& CommandQueue::GetCommandList()
{
	return m_commandList;
}

ComPtr<ID3D12CommandQueue>& CommandQueue::GetCommandQueue()
{
	return m_commandQueue;
}
