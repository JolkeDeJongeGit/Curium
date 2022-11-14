#include <precomp.h>
#include "graphics/win32/WinCommandQueue.h"
#include <graphics/win32/WinDevice.h>
#include <graphics/win32/WinSwapchain.h>

CommandQueue::CommandQueue()
{
	auto device = Device::Get().GetDevice();
	D3D12_COMMAND_QUEUE_DESC description = {};
	description.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	description.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	ThrowIfFailed(device->CreateCommandQueue(&description, IID_PPV_ARGS(&m_commandQueue)));

	ThrowIfFailed(m_commandList.GetList()->Close());
}

CommandQueue::CommandQueue(D3D12_COMMAND_LIST_TYPE _type)
{
}

CommandQueue::~CommandQueue()
{
}
 
void CommandQueue::ExecuteCommandList()
{
	ID3D12CommandList* const commandLists[] = { m_commandList.GetList().Get()};
	m_commandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);
	Swapchain::Get().UpdateFenceValue();
}

void CommandQueue::UploadData(ComPtr<ID3D12Resource> _resource, D3D12_SUBRESOURCE_DATA _subresource)
{
	auto list = m_commandList.GetList();
	auto alloc = m_commandList.GetAllocater();
	ComPtr<ID3D12Device2> device = Device::Get().GetDevice();
	CD3DX12_RESOURCE_BARRIER copyBarrier = CD3DX12_RESOURCE_BARRIER::Transition(_resource.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST);
	CD3DX12_RESOURCE_BARRIER pixelBarrier = CD3DX12_RESOURCE_BARRIER::Transition(_resource.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	ThrowIfFailed(alloc->Reset());
	ThrowIfFailed(list->Reset(alloc.Get(), nullptr));

	list->ResourceBarrier(1, &copyBarrier);

	UINT64 size = GetRequiredIntermediateSize(_resource.Get(), 0, 1);
	ComPtr<ID3D12Resource> intermediate;
	D3D12_HEAP_PROPERTIES properties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(size);

	ThrowIfFailed(device->CreateCommittedResource(
		&properties,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&intermediate)
	));

	UpdateSubresources(list.Get(), _resource.Get(), intermediate.Get(), 0, 0, 1, &_subresource);
	list->ResourceBarrier(1, &pixelBarrier);
	list->Close();

	ExecuteCommandList();
	Swapchain::Get().WaitForFenceValue(m_commandQueue);
}


CommandList& CommandQueue::GetCommandList()
{
	return m_commandList;
}

ComPtr<ID3D12CommandQueue>& CommandQueue::GetCommandQueue()
{
	return m_commandQueue;
}
