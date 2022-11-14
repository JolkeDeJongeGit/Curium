#include "precomp.h"
#include "graphics/win32/WinDevice.h"
#include "graphics/win32/WinCommandQueue.h"
#include "graphics/win32/WinSwapchain.h"
#include "graphics/win32/WinPipelineState.h"
#include "graphics/win32/WinDescriptorHeap.h"
#include "graphics/Renderer.h"
#include <graphics/win32/WinHeapHandler.h>


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
	ComPtr<ID3D12CommandAllocator> commandAllocator = m_commandQueue->GetCommandAllocator();
	ComPtr<ID3D12GraphicsCommandList> commandList = m_commandQueue->GetCommandList();

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
	CD3DX12_RESOURCE_BARRIER presentBarrier = CD3DX12_RESOURCE_BARRIER::Transition(renderTarget, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

	ThrowIfFailed(commandAllocator->Reset());
	ThrowIfFailed(commandList->Reset(commandAllocator.Get(), m_pipelineState->GetPipelineState().Get()));

	commandList->ResourceBarrier(1, &renderTargetBarrier);
	commandList->ClearRenderTargetView(rtvHandle, m_colorRGBA, 0, nullptr);
	commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
	commandList->SetPipelineState(m_pipelineState->GetPipelineState().Get());
	commandList->SetGraphicsRootSignature(m_pipelineState->GetRootSignature().Get());
	commandList->SetDescriptorHeaps(_countof(pDescriptorHeaps), pDescriptorHeaps);

	Draw();

	D3D12_VIEWPORT viewport = { 0.f, 0.f, (float)m_viewportWidth, (float)m_viewportHeight, 0.01f, 500 };
	D3D12_RECT rect = { (long)100, (long)100, (long)100 + (long)m_viewportWidth, (long)100 + (long)m_viewportHeight };
	commandList->RSSetViewports(1, &viewport);
	commandList->RSSetScissorRects(1, &rect);
	commandList->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	commandList->ResourceBarrier(1, &presentBarrier);

	ThrowIfFailed(commandList->Close());

	m_commandQueue->ExecuteCommandList();
	m_swapchain->Present();
	m_swapchain->WaitForFenceValue(m_commandQueue->GetCommandQueue());

	m_frames++;
}

void Renderer::Draw()
{
}

void Renderer::Draw(glm::mat4 _model, std::vector<VertexData> _data)
{
}

void Renderer::Draw(glm::mat4 _model, Mesh _data)
{
}


void Renderer::Terminate()
{
	m_swapchain->WaitForFenceValue(m_commandQueue->GetCommandQueue());
	m_swapchain->~Swapchain();
	//delete m_device;
}
