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
#include "graphics/Camera.h"

namespace Renderer
{
	Device* device = nullptr;
	CommandQueue* command_queue = nullptr;
	Swapchain* swapchain = nullptr;
	PipelineState* pipeline_state = nullptr;

	DescriptorHeap* cbv_heap;
	DescriptorHeap* rtv_heap;
	DescriptorHeap* dsv_heap;

	uint32_t viewport_width = 1920;
	uint32_t viewport_height = 1080;

	constexpr float color_rgba[4] = { 0.2f,0.2f,0.2f,1 };

	Camera camera;

	Mesh* cube;
}

float WinWindow::MouseXOffset;
float WinWindow::MouseYOffset;

void Renderer::Init(const uint32_t inWidth, const uint32_t inHeight)
{
	device = &Device::Get();
	command_queue = &CommandQueue::Get();
	swapchain = &Swapchain::Get();
	pipeline_state = new PipelineState("basic.vertex", "basic.pixel", D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);

	swapchain->Init(static_cast<int>(inWidth), static_cast<int>(inHeight));

	auto heapHandler = HeapHandler::Get();
	heapHandler.CreateHeaps(Swapchain::BackBufferCount);
	// Reference of heaps
	cbv_heap = &heapHandler.GetCbvHeap();
	rtv_heap = &heapHandler.GetRtvHeap();
	dsv_heap = &heapHandler.GetDsvHeap();

	viewport_width = inWidth;
	viewport_height = inHeight;
	cube = new Mesh();

	const Transform transform(glm::vec3(0, 0, 0), glm::vec3(0), glm::vec3(1));
	camera = Camera(transform, static_cast<float>(inWidth) / static_cast<float>(inHeight), 80.f);
}

void Renderer::Render()
{
	const ComPtr<ID3D12GraphicsCommandList> commandList = command_queue->GetCommandList().GetList();
	const UINT backBufferIndex = swapchain->GetCurrentBuffer();
	ID3D12Resource* renderTarget = swapchain->GetCurrentRenderTarget(backBufferIndex).Get();
	const CD3DX12_RESOURCE_BARRIER presentBarrier = CD3DX12_RESOURCE_BARRIER::Transition(renderTarget, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	
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

	auto& heapHandler = HeapHandler::Get();
	// Reference of heaps
	cbv_heap = &heapHandler.GetCbvHeap();
	rtv_heap = &heapHandler.GetRtvHeap();
	dsv_heap = &heapHandler.GetDsvHeap();

	ID3D12DescriptorHeap* pDescriptorHeaps[] = { cbv_heap->GetDescriptorHeap().Get() };

	const UINT backBufferIndex = swapchain->GetCurrentBuffer();
	ID3D12Resource* renderTarget = swapchain->GetCurrentRenderTarget(backBufferIndex).Get();

	const CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtv_heap->GetCpuHandleAt(backBufferIndex);
	const CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsv_heap->GetCpuHandleAt(0);

	const CD3DX12_RESOURCE_BARRIER renderTargetBarrier = CD3DX12_RESOURCE_BARRIER::Transition(renderTarget, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

	ThrowIfFailed(commandAllocator->Reset());
	ThrowIfFailed(commandList->Reset(commandAllocator.Get(), pipeline_state->GetPipelineState().Get()));

	commandList->ResourceBarrier(1, &renderTargetBarrier);
	commandList->ClearRenderTargetView(rtvHandle, color_rgba, 0, nullptr);
	commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
	
	commandList->SetPipelineState(pipeline_state->GetPipelineState().Get());
	commandList->SetGraphicsRootSignature(pipeline_state->GetRootSignature().Get());
	commandList->SetDescriptorHeaps(_countof(pDescriptorHeaps), pDescriptorHeaps);

	const D3D12_VIEWPORT viewport = { 0.f, 0.f, static_cast<float>(viewport_width), static_cast<float>(viewport_height)};
	const D3D12_RECT rect = { 0, 0, static_cast<long>(viewport_width), static_cast<long>(viewport_height) };
	commandList->RSSetViewports(1, &viewport);
	commandList->RSSetScissorRects(1, &rect);
	commandList->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	ImGui::Begin("Triangle Position/Color");
	static float rotationX = 0.0;
	ImGui::SliderFloat("rotationX", &rotationX, 0, 360);
	static float rotationY = 0.0;
	ImGui::SliderFloat("rotationY", &rotationY, 0, 360);
	static float rotationZ = 0.0;
	ImGui::SliderFloat("rotationZ", &rotationZ, 0, 360);
	static float translation[] = {0.0, 0.0, 10.0};
	ImGui::SliderFloat3("position", translation, -10, 10);
	static float scale[] = {2.0, 2.0, 2.0};
	ImGui::SliderFloat3("scale", scale, 0.1f, 100.0f);
	ImGui::End();
	
	glm::mat4 trans = glm::identity<glm::mat4>();
	trans = glm::translate(trans, glm::vec3(translation[0], translation[1], translation[2]));
	trans = glm::rotate(trans, glm::radians(rotationX), glm::vec3(1.0, 0.0, 0.0));
	trans = glm::rotate(trans, glm::radians(rotationY), glm::vec3(0.0, 1.0, 0.0));
	trans = glm::rotate(trans, glm::radians(rotationZ), glm::vec3(0.0, 0.0, 1.0));
	trans = glm::scale(trans, glm::vec3(scale[0], scale[1], scale[2]));
	
	commandList->SetGraphicsRoot32BitConstants(0, 16, glm::value_ptr(camera.GetView()), 0);
	commandList->SetGraphicsRoot32BitConstants(0, 16, glm::value_ptr(camera.GetProjection()), 16);
	commandList->SetGraphicsRoot32BitConstants(0, 16, glm::value_ptr(trans), 32);
	
	cube->Draw(glm::identity<glm::mat4>(), commandList);
	
	Debug::Render();
}

void Renderer::Shutdown()
{
	swapchain->WaitForFenceValue(command_queue->GetCommandQueue());
	swapchain->~Swapchain();
}

Camera* Renderer::GetCamera()
{
	return &camera;
}

Device* WinUtil::GetDevice()
{
	return Renderer::device;
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
		return &HeapHandler::Get().GetCbvHeap();

	case HeapType::DSV:
		return &HeapHandler::Get().GetDsvHeap();

	case HeapType::RTV:
		return &HeapHandler::Get().GetRtvHeap();  // NOLINT(clang-diagnostic-covered-switch-default)

	default:  // NOLINT(clang-diagnostic-covered-switch-default)
		return Renderer::cbv_heap;
	}
}
