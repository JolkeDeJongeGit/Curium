#include "precomp.h"
#include "graphics/win32/WinDevice.h"
#include "graphics/win32/WinCommandQueue.h"
#include "graphics/win32/WinSwapchain.h"
#include "graphics/win32/WinPipelineState.h"
#include "graphics/win32/WinDescriptorHeap.h"
#include "graphics/win32/WinHeapHandler.h"
#include "graphics/win32/WinUtil.h"
#include "platform/win32/WinWindow.h"
#include "Engine.h"
#include "graphics/Renderer.h"
#include <graphics/DebugManager.h>
#include <include/imgui.h>

namespace Renderer
{
	Device* m_device = nullptr;
	CommandQueue* m_commandQueue = nullptr;
	Swapchain* m_swapchain = nullptr;
	PipelineState* m_pipelineState = nullptr;

	DescriptorHeap* m_cbvHeap;
	DescriptorHeap* m_rtvHeap;
	DescriptorHeap* m_dsvHeap;

	uint32_t m_viewportWidth = 1920;
	uint32_t m_viewportHeight = 1080;

	const float m_colorRGBA[4] = { 0.5f,0.1f,0.1f,1 };
}

float WinWindow::MouseXOffset;
float WinWindow::MouseYOffset;

void Renderer::Init(uint32_t _width, uint32_t _height)
{
	m_device = &Device::Get();
	m_commandQueue = &CommandQueue::Get();
	m_swapchain = &Swapchain::Get();
	m_pipelineState = new PipelineState("basic.vertex", "basic.pixel", D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);

	m_swapchain->Init(_width, _height);

	auto heapHandler = HeapHandler::Get();
	heapHandler.CreateHeaps(Swapchain::BackBufferCount);
	// Reference of heaps
	m_cbvHeap = &heapHandler.GetCbvHeap();
	m_rtvHeap = &heapHandler.GetRtvHeap();
	m_dsvHeap = &heapHandler.GetDsvHeap();

	m_viewportWidth = _width;
	m_viewportHeight = _height;
}

void Renderer::Render()
{
	ComPtr<ID3D12GraphicsCommandList> commandList = m_commandQueue->GetCommandList().GetList();
	UINT backBufferIndex = m_swapchain->GetCurrentBuffer();
	ID3D12Resource* renderTarget = m_swapchain->GetCurrentRenderTarget(backBufferIndex).Get();
	CD3DX12_RESOURCE_BARRIER presentBarrier = CD3DX12_RESOURCE_BARRIER::Transition(renderTarget, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	
	commandList->ResourceBarrier(1, &presentBarrier);
	ThrowIfFailed(commandList->Close());

	m_commandQueue->ExecuteCommandList();
	m_swapchain->Present();
	m_swapchain->WaitForFenceValue(m_commandQueue->GetCommandQueue());
}

void Renderer::Update()
{
	PROFILE_FUNCTION();
	ComPtr<ID3D12CommandAllocator> commandAllocator = m_commandQueue->GetCommandList().GetAllocater();
	ComPtr<ID3D12GraphicsCommandList> commandList = m_commandQueue->GetCommandList().GetList();

	auto heapHandler = HeapHandler::Get();
	// Reference of heaps
	m_cbvHeap = &heapHandler.GetCbvHeap();
	m_rtvHeap = &heapHandler.GetRtvHeap();
	m_dsvHeap = &heapHandler.GetDsvHeap();

	ID3D12DescriptorHeap* pDescriptorHeaps[] = { m_cbvHeap->GetDescriptorHeap().Get() };

	UINT backBufferIndex = m_swapchain->GetCurrentBuffer();
	ID3D12Resource* renderTarget = m_swapchain->GetCurrentRenderTarget(backBufferIndex).Get();

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_rtvHeap->GetCPUHandleAt(backBufferIndex);
	CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle = m_dsvHeap->GetCPUHandleAt(0);

	CD3DX12_RESOURCE_BARRIER renderTargetBarrier = CD3DX12_RESOURCE_BARRIER::Transition(renderTarget, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

	ThrowIfFailed(commandAllocator->Reset());
	ThrowIfFailed(commandList->Reset(commandAllocator.Get(), m_pipelineState->GetPipelineState().Get()));

	commandList->ResourceBarrier(1, &renderTargetBarrier);
	commandList->ClearRenderTargetView(rtvHandle, m_colorRGBA, 0, nullptr);
	commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
	commandList->SetPipelineState(m_pipelineState->GetPipelineState().Get());
	commandList->SetGraphicsRootSignature(m_pipelineState->GetRootSignature().Get());
	commandList->SetDescriptorHeaps(_countof(pDescriptorHeaps), pDescriptorHeaps);

	D3D12_VIEWPORT viewport = { 0.f, 0.f, static_cast<float>(m_viewportWidth), static_cast<float>(m_viewportHeight), 0.01f, 500 };
	D3D12_RECT rect = { static_cast<long>(100), static_cast<long>(100), static_cast<long>(100) + static_cast<long>(m_viewportWidth), static_cast<long>(100) + static_cast<long>(m_viewportHeight) };
	commandList->RSSetViewports(1, &viewport);
	commandList->RSSetScissorRects(1, &rect);
	commandList->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	Debug::Render();
	// Draw
}

void Renderer::Shutdown()
{
	m_swapchain->WaitForFenceValue(m_commandQueue->GetCommandQueue());
	m_swapchain->~Swapchain();
}

Device* WinUtil::GetDevice()
{
	return Renderer::m_device;
}

CommandQueue* WinUtil::GetCommandQueue()
{
	return Renderer::m_commandQueue;
}

WinWindow* WinUtil::GetWindow()
{
	return Engine::GetWindow();
}

DescriptorHeap* WinUtil::GetDescriptorHeap(HeapType type)
{
	switch (type)
	{
	case HeapType::CBV_SRV_UAV:
		return &HeapHandler::Get().GetCbvHeap();
		break;

	case HeapType::DSV:
		return &HeapHandler::Get().GetDsvHeap();
		break;

	case HeapType::RTV:
		return &HeapHandler::Get().GetRtvHeap();
		break;

	default:
		return Renderer::m_cbvHeap;
		break;
	}
}
