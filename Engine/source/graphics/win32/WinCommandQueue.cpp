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
	ThrowIfFailed(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator)));
	ThrowIfFailed(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator.Get(), nullptr, IID_PPV_ARGS(&m_commandList)));
	ThrowIfFailed(m_commandList->Close());
}

CommandQueue::~CommandQueue()
{
}

void CommandQueue::ExecuteCommandList()
{
	ID3D12CommandList* const commandLists[] = { m_commandList.Get() };
	m_commandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);
	Swapchain::Get().UpdateFenceValue();
}

void CommandQueue::UploadData(ComPtr<ID3D12Resource> resource, D3D12_SUBRESOURCE_DATA subresource)
{
	ComPtr<ID3D12Device2> device = Device::Get().GetDevice();
	CD3DX12_RESOURCE_BARRIER copyBarrier = CD3DX12_RESOURCE_BARRIER::Transition(resource.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST);
	CD3DX12_RESOURCE_BARRIER pixelBarrier = CD3DX12_RESOURCE_BARRIER::Transition(resource.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	ThrowIfFailed(m_commandAllocator->Reset());
	ThrowIfFailed(m_commandList->Reset(m_commandAllocator.Get(), nullptr));

	m_commandList->ResourceBarrier(1, &copyBarrier);

	UINT64 size = GetRequiredIntermediateSize(resource.Get(), 0, 1);
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

	UpdateSubresources(m_commandList.Get(), resource.Get(), intermediate.Get(), 0, 0, 1, &subresource);
	m_commandList->ResourceBarrier(1, &pixelBarrier);
	m_commandList->Close();

	ExecuteCommandList();
	Swapchain::Get().WaitForFenceValue(m_commandQueue);
}

ComPtr<ID3D12CommandAllocator>& CommandQueue::GetCommandAllocator()
{
	return m_commandAllocator;
}

ComPtr<ID3D12GraphicsCommandList>& CommandQueue::GetCommandList()
{
	return m_commandList;
}

ComPtr<ID3D12CommandQueue>& CommandQueue::GetCommandQueue()
{
	return m_commandQueue;
}
