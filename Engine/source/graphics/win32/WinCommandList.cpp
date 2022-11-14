#include <precomp.h>
#include "graphics/win32/WinCommandList.h"
#include <graphics/win32/WinDevice.h>

CommandList::CommandList()
{
	auto device = Device::Get().GetDevice();
	ThrowIfFailed(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator)));
	ThrowIfFailed(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator.Get(), nullptr, IID_PPV_ARGS(&m_commandList)));
}

CommandList::CommandList(D3D12_COMMAND_LIST_TYPE _type)
{
	auto device = Device::Get().GetDevice();
	ThrowIfFailed(device->CreateCommandAllocator(_type, IID_PPV_ARGS(&m_commandAllocator)));
	ThrowIfFailed(device->CreateCommandList(0, _type, m_commandAllocator.Get(), nullptr, IID_PPV_ARGS(&m_commandList)));
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
