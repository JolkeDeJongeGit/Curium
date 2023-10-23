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
#include "Components/GameObject.h"
#include "graphics/Camera.h"
#include <graphics/win32/WinBuffer.h>

namespace Renderer
{
	Device* device = nullptr;
	CommandQueue* command_queue = nullptr;
	Swapchain* swapchain = nullptr;
	PipelineState* pipeline_state = nullptr;

	DescriptorHeap* cbv_heap;
	DescriptorHeap* rtv_heap;
	DescriptorHeap* dsv_heap;

	HeapHandler* heap_handler;

	uint32_t viewport_width = 1920;
	uint32_t viewport_height = 1080;

	constexpr float color_rgba[4] = { 0.2f,0.2f,0.2f,1 };

	Camera camera;

	Buffer m_domainConstant;
	// @TODO::Add this to a scene class
	std::unordered_map<std::string, GameObject> scene;

	std::vector<int> rootConstants{ 64, 64 };
}



void Renderer::Init(const uint32_t inWidth, const uint32_t inHeight)
{
	device = new Device;
	command_queue = new CommandQueue;
	swapchain = new Swapchain;
	
	heap_handler = new HeapHandler;
	heap_handler->CreateHeaps(Swapchain::BackBufferCount);
	// Reference of heaps
	cbv_heap = heap_handler->GetCbvHeap();
	rtv_heap = heap_handler->GetRtvHeap();
	dsv_heap = heap_handler->GetDsvHeap();
	pipeline_state = new PipelineState("basic.vertex", "basic.pixel", D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH);

	viewport_width = inWidth;
	viewport_height = inHeight;

	// Don't know but I have to do this to fix imgui
	cbv_heap->GetNextIndex();

	swapchain->Init(static_cast<int>(inWidth), static_cast<int>(inHeight));
	
	const Transform transform(glm::vec3(0, 0, 0), glm::vec3(0), glm::vec3(1));
	camera = Camera(transform, static_cast<float>(inWidth) / static_cast<float>(inHeight), 80.f);

	// World creation
	const Transform transformWorld(glm::vec3(0, -1.f, 2.f), glm::vec3(0), glm::vec3(100.f));
	scene.insert(std::pair<std::string, GameObject>("World", GameObject(transformWorld, {Mesh(false)})));
	scene["World"].Init();

	m_domainConstant.CreateConstantBuffer(16 * sizeof(float));
}

void Renderer::Render()
{
	const ComPtr<ID3D12GraphicsCommandList> commandList = command_queue->GetCommandList().GetList();
	const UINT backBufferIndex = swapchain->GetCurrentBuffer();
	ID3D12Resource* renderTarget = swapchain->GetCurrentRenderTarget(backBufferIndex).Get();
	const CD3DX12_RESOURCE_BARRIER presentBarrier = CD3DX12_RESOURCE_BARRIER::Transition(renderTarget, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	
	Debug::Render(commandList);

	commandList->ResourceBarrier(1, &presentBarrier);
	ThrowIfFailed(commandList->Close());

	command_queue->ExecuteCommandList();
	swapchain->Present();
	swapchain->WaitForFenceValue(command_queue->GetCommandQueue());
}

void Renderer::Update()
{
	PROFILE_FUNCTION()
	const ComPtr<ID3D12CommandAllocator> commandAllocator = command_queue->GetCommandList().GetAllocater();
	const ComPtr<ID3D12GraphicsCommandList> commandList = command_queue->GetCommandList().GetList();

	Debug::EditProperties(scene);

	ID3D12DescriptorHeap* pDescriptorHeaps[] = { cbv_heap->GetDescriptorHeap().Get() };

	const UINT backBufferIndex = swapchain->GetCurrentBuffer();
	ID3D12Resource* renderTarget = swapchain->GetCurrentRenderTarget(backBufferIndex).Get();

	const CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtv_heap->GetCpuHandleAt(backBufferIndex);
	const CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsv_heap->GetCpuHandleAt(0);

	
	const CD3DX12_RESOURCE_BARRIER renderTargetBarrier = CD3DX12_RESOURCE_BARRIER::Transition(renderTarget, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

	ThrowIfFailed(commandAllocator->Reset());
	ThrowIfFailed(commandList->Reset(commandAllocator.Get(), nullptr));

	commandList->ResourceBarrier(1, &renderTargetBarrier);

	commandList->ClearRenderTargetView(rtvHandle, color_rgba, 0, nullptr);
	commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
	
	if (Debug::IsWireframeMode())
	{
		commandList->SetPipelineState(pipeline_state->GetWireframePipelineState().Get());
	}
	else
	{
		commandList->SetPipelineState(pipeline_state->GetPipelineState().Get());
	}

	commandList->SetGraphicsRootSignature(pipeline_state->GetRootSignature().Get());
	commandList->SetDescriptorHeaps(_countof(pDescriptorHeaps), pDescriptorHeaps);

	const D3D12_VIEWPORT viewport = { 0.f, 0.f, static_cast<float>(viewport_width), static_cast<float>(viewport_height), 0.0f, 1.0};
	const D3D12_RECT rect = { 0, 0, static_cast<long>(viewport_width), static_cast<long>(viewport_height) };
	commandList->RSSetViewports(1, &viewport);
	commandList->RSSetScissorRects(1, &rect);
	commandList->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);
	
	commandList->SetGraphicsRoot32BitConstants(1, static_cast<UINT>(rootConstants.size()), rootConstants.data(), 0);

	for(auto& [name, gameobject] : scene)
	{
		m_domainConstant.UpdateBuffer(&(camera.GetProjection() * camera.GetView() * gameobject.GetTransform().GetModelMatrix())[0]);
		m_domainConstant.SetGraphicsRootConstantBufferView(commandList, 0);

		for (const Mesh& mesh : gameobject.GetMeshes())
		{
			mesh.Draw(commandList);
		}
	}
}

void Renderer::Shutdown()
{
	delete device;
	delete heap_handler;
	delete swapchain;
	delete command_queue;
	device = nullptr;
	heap_handler = nullptr;
	command_queue = nullptr;
	swapchain = nullptr;
}


Camera* Renderer::GetCamera()
{
	return &camera;
}

//TODO:: REMOVE THIS DEBUG ONLY
std::vector<int>& Renderer::GetRootConstants()
{
	return rootConstants;
}

Device* WinUtil::GetDevice()
{
	return Renderer::device;
}

Swapchain* WinUtil::GetSwapchain()
{
	return Renderer::swapchain;
}

CommandQueue* WinUtil::GetCommandQueue()
{
	return Renderer::command_queue;
}

WinWindow* WinUtil::GetWindow()
{
	return Engine::GetWindow();
}

DescriptorHeap* WinUtil::GetDescriptorHeap(const HeapType inType)
{
	switch (inType)
	{
	case HeapType::CBV_SRV_UAV:
		return Renderer::cbv_heap;

	case HeapType::DSV:
		return Renderer::dsv_heap;

	case HeapType::RTV:
		return Renderer::rtv_heap;  // NOLINT(clang-diagnostic-covered-switch-default)

	default:  // NOLINT(clang-diagnostic-covered-switch-default)
		return Renderer::cbv_heap;
	}
}
