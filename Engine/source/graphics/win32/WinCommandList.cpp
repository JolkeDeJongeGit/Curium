#include <precomp.h>
#include "graphics/win32/WinCommandList.h"
#include <graphics/win32/WinDevice.h>

CommandList::CommandList()
{
	const auto device = Device::Get().GetDevice();
	ThrowIfFailed(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator)));
	ThrowIfFailed(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator.Get(), nullptr, IID_PPV_ARGS(&m_commandList)));
}

CommandList::CommandList(const D3D12_COMMAND_LIST_TYPE inType)
{
	const auto device = Device::Get().GetDevice();
	ThrowIfFailed(device->CreateCommandAllocator(inType, IID_PPV_ARGS(&m_commandAllocator)));
	ThrowIfFailed(device->CreateCommandList(0, inType, m_commandAllocator.Get(), nullptr, IID_PPV_ARGS(&m_commandList)));
}

CommandList::~CommandList()
{
}

ComPtr<ID3D12CommandAllocator>& CommandList::GetAllocater()
{
	return m_commandAllocator;
}

ComPtr<ID3D12GraphicsCommandList>& CommandList::GetList()
{
	return m_commandList;
}
