#include "precomp.h"
#include "graphics/win32/WinDevice.h"
#include "graphics/win32/WinCommandQueue.h"
#include "graphics/win32/WinSwapchain.h"
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
#include <core/ImGuiLayer.h>
#include "core/Scene.h"
#include <components/gameobjects/Planet.h>
#include <components/gameobjects/Light.h>
#include "graphics/win32/Pipeline/WinPipelineStateScreen.h"
#include <graphics/ShaderManager.h>
#include <graphics/win32/Pipeline/WinPipelineStateSky.h>
#include <graphics/win32/Pipeline/WinPipelineStateCompute.h>
#include "common/AssetManager.h"

namespace Renderer
{
	Device* device = nullptr;
	CommandQueue* command_queue = nullptr;
	Swapchain* swapchain = nullptr;
	PipelineState* pipeline_state = nullptr;

	PipelineStateScreen* pipeline_screen = nullptr;
	PipelineStateSky* pipeline_sky = nullptr;
	PipelineStateCompute* pipeline_compute = nullptr;

	DescriptorHeap* cbv_heap;
	DescriptorHeap* rtv_heap;
	DescriptorHeap* dsv_heap;

	ShaderManager* shader_manager;

	HeapHandler* heap_handler;

	uint32_t viewport_width = 1920;
	uint32_t viewport_height = 1080;

	constexpr float color_rgba[4] = { 0.2f,0.2f,0.2f,1 };

	Camera camera;

	std::vector<int> root_constants{ 64, 64 };

	std::vector<Mesh> remove_meshes;

	glm::vec3 light_color(1.f,1.f,1.f);
	glm::vec3 light_dir(0.f,.5f,1.f);

	void RenderImGui();
}

void Renderer::Init(const uint32_t inWidth, const uint32_t inHeight)
{
	device = new Device;
	command_queue = new CommandQueue;
	swapchain = new Swapchain;
	
	heap_handler = new HeapHandler;

	heap_handler->CreateHeaps(Swapchain::BackBufferCount + 1);

	// Reference of heaps
	cbv_heap = heap_handler->GetCbvHeap();
	rtv_heap = heap_handler->GetRtvHeap();
	dsv_heap = heap_handler->GetDsvHeap();

	shader_manager = new ShaderManager();
	shader_manager->Init();


	viewport_width = inWidth;
	viewport_height = inHeight;

	// Don't know but I have to do this to fix imgui
	cbv_heap->GetNextIndex();

	command_queue->OpenCommandList();

	pipeline_sky = new PipelineStateSky("sky.vertex", "sky.pixel", D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
	pipeline_state = new PipelineState("basic.vertex", "basic.pixel", D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH);
	pipeline_screen = new PipelineStateScreen("screen.vertex", "screen.pixel", D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
	pipeline_compute = new PipelineStateCompute("basic.compute");

	swapchain->Init(static_cast<int>(inWidth), static_cast<int>(inHeight));
	
	const Transform transform(glm::vec3(0, 0, 0), glm::vec3(0), glm::vec3(1));
	camera = Camera(transform, static_cast<float>(inWidth) / static_cast<float>(inHeight), 80.f);

	// @TODO::Needs to load in scene 
	Planet* ter = new Planet(8, 100000);
	const Transform transformWorld(glm::vec3(0, -103000.f, 0.f), glm::vec3(0), glm::vec3(1.f));
	ter->SetTransform(transformWorld);
	Scene::AddSceneObject("World", ter);

	Light* sun = new Light();
	const Transform transformWorld1(glm::vec3(0, 0.f, 0.f), glm::vec3(0), glm::vec3(1.f));
	sun->SetTransform(transformWorld1);
	Scene::AddSceneObject("Sun", sun);

	command_queue->CloseCommandList();
}

void Renderer::Update()
{
	PROFILE_FUNCTION()
	const ComPtr<ID3D12GraphicsCommandList>& commandList = command_queue->GetCommandList().GetList();
	ID3D12DescriptorHeap* pDescriptorHeaps[] = { cbv_heap->GetDescriptorHeap().Get() };

	ID3D12Resource* renderTarget = swapchain->GetRenderTextureBuffer().Get();

	const CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtv_heap->GetCpuHandleAt(swapchain->m_renderTextureHeapID);
	const CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsv_heap->GetCpuHandleAt(0);

	command_queue->OpenCommandList();

	commandList->SetDescriptorHeaps(_countof(pDescriptorHeaps), pDescriptorHeaps);
	pipeline_compute->Render(commandList, 2048 * 0.0625f, 2048 * 0.0625f, 1);

	const CD3DX12_RESOURCE_BARRIER renderTargetBarrier = CD3DX12_RESOURCE_BARRIER::Transition(renderTarget, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);
	commandList->ResourceBarrier(1, &renderTargetBarrier);

	commandList->ClearRenderTargetView(rtvHandle, color_rgba, 0, nullptr);
	commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
	
	const D3D12_VIEWPORT viewport = { 0.f, 0.f, static_cast<float>(viewport_width), static_cast<float>(viewport_height), 0.0f, 1.0f };
	const D3D12_RECT rect = { 0, 0, static_cast<long>(viewport_width), static_cast<long>(viewport_height) };

	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);

	commandList->RSSetViewports(1, &viewport);
	commandList->RSSetScissorRects(1, &rect);
	commandList->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);

	commandList->SetGraphicsRootSignature(pipeline_state->GetRootSignature().Get());
	if (Debug::IsWireframeMode())
	{
		commandList->SetPipelineState(pipeline_state->GetWireframePipelineState().Get());
	}
	else
	{
		commandList->SetPipelineState(pipeline_state->GetPipelineState().Get());
	}

	// - Bind params
	commandList->SetGraphicsRoot32BitConstants(2, static_cast<UINT>(root_constants.size()), root_constants.data(), 0);
	commandList->SetGraphicsRootDescriptorTable(4, cbv_heap->GetGpuHandleAt(pipeline_compute->m_sharedIndex));

	glm::mat4 vp = camera.GetProjection() * camera.GetView();
	commandList->SetGraphicsRoot32BitConstants(0, 16, &vp, 0);
	commandList->SetGraphicsRoot32BitConstants(1, 4, &camera.GetTransform().GetPosition(), 0);
	commandList->SetGraphicsRoot32BitConstants(1, 4, &light_dir, 4);
	commandList->SetGraphicsRoot32BitConstants(1, 4, &light_color, 8);

	for(auto& [name, gameobject] : Scene::AllSceneObjects())
	{
		commandList->SetGraphicsRoot32BitConstants(0, 16, &gameobject->GetTransform().GetModelMatrix(), 16);

		if (gameobject->GetMeshes().size() > 0)
		{
			auto& bozo = gameobject->GetMeshes()[0];
			if (auto heightmapTexture = bozo.m_textureData.find("heightmap"); heightmapTexture != bozo.m_textureData.end())
			{
				auto& texture = heightmapTexture->second;
				auto descriptorIndex = texture.GetDescriptorIndex();
				commandList->SetGraphicsRootDescriptorTable(3, WinUtil::GetDescriptorHeap(HeapType::CBV_SRV_UAV)->GetGpuHandleAt(descriptorIndex));
			}
		}

		for (auto& mesh : gameobject->GetMeshes())
		{
			if(!mesh.m_cull)
				mesh.Draw(commandList);
		}
	}
}

void Renderer::Render()
{
	const ComPtr<ID3D12GraphicsCommandList> commandlist = command_queue->GetCommandList().GetList();
	ID3D12Resource* renderTarget = swapchain->GetRenderTextureBuffer().Get();
	pipeline_sky->Render(commandlist);
	//pipeline_screen->Render(commandlist);

	const CD3DX12_RESOURCE_BARRIER RTToShaderResourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(renderTarget, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	commandlist->ResourceBarrier(1, &RTToShaderResourceBarrier);

	ImGuiLayer::NewFrame();

	RenderImGui();

	ThrowIfFailed(commandlist->Close());
	command_queue->ExecuteCommandList();

	swapchain->Present();

	for (Mesh& mesh : remove_meshes)
	{
		mesh.Shutdown();
	}

	remove_meshes.clear();
}

void Renderer::RenderImGui()
{
	const ComPtr<ID3D12GraphicsCommandList> commandlist = command_queue->GetCommandList().GetList();

	const UINT backBufferIndex = swapchain->GetCurrentBuffer();
	ID3D12Resource* renderTarget = swapchain->GetCurrentRenderTarget(backBufferIndex).Get();
	const CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtv_heap->GetCpuHandleAt(backBufferIndex);

	const CD3DX12_RESOURCE_BARRIER renderTargetBarrier = CD3DX12_RESOURCE_BARRIER::Transition(renderTarget, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	commandlist->ResourceBarrier(1, &renderTargetBarrier);

	commandlist->OMSetRenderTargets(1, &rtvHandle, false, nullptr);

	ImGuiLayer::UpdateWindow();
	ImGuiLayer::Render(commandlist.Get());

	const CD3DX12_RESOURCE_BARRIER presentImguiBarrier = CD3DX12_RESOURCE_BARRIER::Transition(renderTarget, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	commandlist->ResourceBarrier(1, &presentImguiBarrier);
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

void Renderer::Resize(uint32_t inWidth, uint32_t inHeight)
{
	viewport_width = inWidth;
	viewport_height = inHeight;
}

void Renderer::DrawCameraPropertyWindow()
{
	ImGui::Begin("Camera Properties", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus);
	ImGui::PushID("C");
	ImGui::Text("Camera Speed: ");
	ImGui::SameLine();
	ImGui::SliderFloat(" ", &camera.GetMovementSpeed(), 8.5, 400000);
	ImGui::PopID();

	ImGui::PushID("R");
	float farplane = camera.GetFarPlane();
	ImGui::Text("Render Distance: ");
	ImGui::SameLine();
	ImGui::SliderFloat(" ", &farplane, 1000.0f, 5000000.0f);
	camera.SetFarPlane(farplane);
	ImGui::PopID();

	ImGui::PushID("R4");
	ImGui::Text("Light Color: ");
	ImGui::SameLine();
	ImGui::ColorEdit3(" ", &light_color[0]);
	ImGui::PopID();

	ImGui::PushID("R3");
	ImGui::Text("Light Direction: ");
	ImGui::SameLine();
	ImGui::DragFloat3(" ", &light_dir[0], 0.01f, -1.f, 1.f);
	ImGui::PopID();
	ImGui::End();
}

void Renderer::AddRemovedMesh(Mesh inMesh)
{
	remove_meshes.push_back(inMesh);
}

Camera* Renderer::GetCamera()
{
	return &camera;
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

ShaderManager* WinUtil::GetShaderManager()
{
	return Renderer::shader_manager;
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
