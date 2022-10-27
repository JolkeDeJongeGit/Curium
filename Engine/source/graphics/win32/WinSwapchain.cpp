#include <precomp.h>
#include "graphics/win32/WinSwapchain.h"
#include <graphics/win32/WinDevice.h>

void SwapChain::Init(int _width, int _height)
{
	auto device = Device::Get().GetDevice();
	SetupSwapchain(_width, _height);

	ThrowIfFailed(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));

	m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	if (m_fenceEvent == nullptr)
		ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
}

SwapChain::SwapChain()
{
}

SwapChain::~SwapChain()
{
	CloseHandle(m_fenceEvent);
}

const uint32_t SwapChain::GetCurrentBuffer()
{
	return m_currentBuffer;
}

ComPtr<IDXGISwapChain4>& SwapChain::GetSwapchain()
{
	return m_swapchain;
}

void SwapChain::ResizeBuffer(int _width, int _height)
{
}

void SwapChain::UpdateFenceValue()
{
	m_fenceValue++;
}

void SwapChain::WaitForFenceValue(ComPtr<ID3D12CommandQueue>& _commandQueue)
{
	ThrowIfFailed(_commandQueue->Signal(m_fence.Get(), m_fenceValue));

	if (m_fence->GetCompletedValue() < m_fenceValue)
	{
		ThrowIfFailed(m_fence->SetEventOnCompletion(m_fenceValue, m_fenceEvent));
		WaitForSingleObject(m_fenceEvent, INFINITE);
	}
}

ComPtr<ID3D12Fence>& SwapChain::GetFence()
{
	return m_fence;
}

void SwapChain::SetupSwapchain(int _width, int _height)
{
}
