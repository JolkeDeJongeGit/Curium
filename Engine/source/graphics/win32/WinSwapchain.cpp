#include <precomp.h>
#include "graphics/win32/WinSwapchain.h"
#include <graphics/win32/WinDevice.h>
#include <platform/win32/WinWindow.h>
#include <graphics/win32/WinCommandQueue.h>
#include <graphics/win32/WinHeapHandler.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#define GLFW_NATIVE_INCLUDE_NONE
#include <GLFW3/include/GLFW/glfw3native.h>
#include "Engine.h"
#include "graphics/win32/WinUtil.h"

void Swapchain::Init(const int inWidth, const int inHeight)
{
	const ComPtr<ID3D12Device2> device = WinUtil::GetDevice()->GetDevice();

	SetupSwapchain(inWidth, inHeight);
	SetupDepthBuffer(inWidth, inHeight);

	ThrowIfFailed(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));

	m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);

	if (m_fenceEvent == nullptr)
		ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
}

Swapchain::Swapchain() = default;


Swapchain::~Swapchain()
{
	CloseHandle(m_fenceEvent);
}

uint32_t Swapchain::GetCurrentBuffer() const
{
	return m_currentBuffer;
}

ComPtr<IDXGISwapChain4>& Swapchain::GetSwapchain()
{
	return m_swapchain;
}

void Swapchain::ResizeBuffer(int _width, int _height) const
{
	m_swapchain->ResizeBuffers(BackBufferCount, _width, _height,
		DXGI_FORMAT_R8G8B8A8_UNORM, 0);
}

void Swapchain::Present()
{
	m_swapchain->Present(1, 0);
	m_currentBuffer = m_swapchain->GetCurrentBackBufferIndex();
}

void Swapchain::UpdateFenceValue()
{
	m_fenceValue++;
}

void Swapchain::WaitForFenceValue(const ComPtr<ID3D12CommandQueue>& inCommandQueue) const
{
	ThrowIfFailed(inCommandQueue->Signal(m_fence.Get(), m_fenceValue));

	if (m_fence->GetCompletedValue() < m_fenceValue)
	{
		ThrowIfFailed(m_fence->SetEventOnCompletion(m_fenceValue, m_fenceEvent));
		WaitForSingleObject(m_fenceEvent, INFINITE);
	}
}

ComPtr<ID3D12Fence>& Swapchain::GetFence()
{
	return m_fence;
}

ComPtr<ID3D12Resource>& Swapchain::GetCurrentRenderTarget(const uint32_t inIndex)
{
	return m_renderTargets[inIndex];
}

ComPtr<ID3D12Resource>& Swapchain::GetDepthBuffer()
{
	return m_depthBuffer;
}

void Swapchain::SetupSwapchain(const int inWidth, const int inHeight)
{
	const Device* device =  WinUtil::GetDevice();
	const ComPtr<ID3D12Device2>& devices = device->GetDevice();
	const ComPtr<IDXGIFactory4>& factory = device->GetFactory();
	const HWND hwnd = glfwGetWin32Window(Engine::GetWindow()->GetWindow());
	const ComPtr<ID3D12CommandQueue>& command = WinUtil::GetCommandQueue()->GetCommandQueue();
	DescriptorHeap* heap = WinUtil::GetDescriptorHeap(HeapType::RTV);

	DXGI_SWAP_CHAIN_DESC1 swapchainDesc = {};
	swapchainDesc.BufferCount = BackBufferCount;
	swapchainDesc.Width = inWidth;
	swapchainDesc.Height = inHeight;
	swapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapchainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapchainDesc.SampleDesc.Count = 1;
	
	IDXGISwapChain1* swapchain;
	ThrowIfFailed(factory->CreateSwapChainForHwnd(command.Get(), hwnd, &swapchainDesc, nullptr, nullptr, &swapchain));
	ThrowIfFailed(factory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER));
	ThrowIfFailed(swapchain->QueryInterface(IID_PPV_ARGS(&m_swapchain)));

	//delete swapchain;
	m_currentBuffer = m_swapchain->GetCurrentBackBufferIndex();

	// This is setting up the render targets.
	for (uint32_t i = 0; i < BackBufferCount; i++)
	{
		const CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle = heap->GetCpuHandleAt(heap->GetNextIndex());
		ThrowIfFailed(m_swapchain->GetBuffer(i, IID_PPV_ARGS(&m_renderTargets[i])));
		devices.Get()->CreateRenderTargetView(m_renderTargets[i].Get(), nullptr, rtvHandle);
	}
}

void Swapchain::SetupDepthBuffer(const int inWidth, const int inHeight)
{
	const ComPtr<ID3D12Device2>& devices = WinUtil::GetDevice()->GetDevice();
	const DescriptorHeap* heap =  WinUtil::GetDescriptorHeap(HeapType::DSV);

	D3D12_CLEAR_VALUE optimizedClearValue = {};
	optimizedClearValue.Format = DXGI_FORMAT_D32_FLOAT;
	optimizedClearValue.DepthStencil = { 1.0f, 0 };

	const CD3DX12_HEAP_PROPERTIES properties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	const CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, inWidth, inHeight,
	                                                                        1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

	ThrowIfFailed(devices.Get()->CreateCommittedResource(
		&properties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&optimizedClearValue,
		IID_PPV_ARGS(&m_depthBuffer)
	));

	D3D12_DEPTH_STENCIL_VIEW_DESC dsv = {};
	dsv.Format = DXGI_FORMAT_D32_FLOAT;
	dsv.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsv.Texture2D.MipSlice = 0;
	dsv.Flags = D3D12_DSV_FLAG_NONE;
	devices.Get()->CreateDepthStencilView(m_depthBuffer.Get(), &dsv,
		heap->GetCpuHandleAt(0));
}
