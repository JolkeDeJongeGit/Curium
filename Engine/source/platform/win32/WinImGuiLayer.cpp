#include "precomp.h"
#include "include/imgui_impl_glfw.h"
#include "include/imgui_impl_dx12.h"
#include "include/ImGuizmo.h"

#include <GLFW3/include/GLFW/glfw3.h>
#include "platform/Window.h"

#include "graphics/win32/WinUtil.h"
#include "platform/win32/WinWindow.h"
#include "graphics/win32/WinDevice.h"
#include "graphics/win32/WinDescriptorHeap.h"
#include "graphics/win32/WinCommandQueue.h"
#include "graphics/win32/WinSwapchain.h"
#include "graphics/Camera.h"
#include "graphics/Renderer.h"

#include "Engine.h"
#include "core/ImGuiLayer.h"

namespace ImGuiLayer
{
    ComPtr<ID3D12Resource> imgui_render_target;
    ID3D12DescriptorHeap* rtv_heap;
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtv_handle;
    void CreateRenderTarget();
};

void ImGuiLayer::CreateRenderTarget()
{
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    auto& devices = WinUtil::GetDevice()->GetDevice();

    // Describe and create a render target texture
    D3D12_RESOURCE_DESC textureDesc = {};
    textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    textureDesc.Width = static_cast<UINT>(io.DisplaySize.x);
    textureDesc.Height = static_cast<UINT>(io.DisplaySize.y);
    textureDesc.DepthOrArraySize = 1;
    textureDesc.MipLevels = 1;
    textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // Adjust format as needed
    textureDesc.SampleDesc.Count = 1;
    textureDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    textureDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

    D3D12_HEAP_PROPERTIES heapProps = {};
    heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;

    devices->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &textureDesc,
        D3D12_RESOURCE_STATE_RENDER_TARGET,
        nullptr,
        IID_PPV_ARGS(&imgui_render_target)
    );

    // Create a render target view
    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
    rtvHeapDesc.NumDescriptors = 1;
    rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

    devices->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&rtv_heap));

    D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
    rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // Match the texture format
    rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

    rtv_handle = rtv_heap->GetCPUDescriptorHandleForHeapStart();
    devices->CreateRenderTargetView(imgui_render_target.Get(), &rtvDesc, rtv_handle);
}

void ImGuiLayer::_Init()
{
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.WantCaptureMouse = true;
    io.DisplaySize = ImVec2(static_cast<float>(Engine::GetWindow()->GetWidth()), static_cast<float>(Engine::GetWindow()->GetHeight()));

    io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
    io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;
    io.BackendFlags |= ImGuiBackendFlags_HasMouseHoveredViewport;

    float xs;
    float ys;
    glfwGetWindowContentScale(Engine::GetWindow()->GetWindow(), &xs, &ys);
    uiScale = (xs + ys) / 2.0f;

#ifdef _DEBUG
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
#else	
    io.ConfigFlags &= ~ImGuiConfigFlags_NavEnableKeyboard;
#endif

    // Initialize ImGui for GLFW
    auto window = Engine::GetWindow()->GetWindow();
    ImGui_ImplGlfw_InitForOther(window, true);

    // Initialize ImGui for DirectX 12
    auto& devices = WinUtil::GetDevice()->GetDevice();
    ID3D12DescriptorHeap* srvHeap = WinUtil::GetDescriptorHeap(HeapType::CBV_SRV_UAV)->GetDescriptorHeap().Get();

    ImGui_ImplDX12_Init(
        devices.Get(),
        Swapchain::BackBufferCount,
        DXGI_FORMAT_R8G8B8A8_UNORM,
        srvHeap,
        srvHeap->GetCPUDescriptorHandleForHeapStart(),
        srvHeap->GetGPUDescriptorHandleForHeapStart()
    );

    // Create the ImGui render target
    CreateRenderTarget();
}

void ImGuiLayer::NewFrame()
{
    ImGui_ImplDX12_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGuizmo::BeginFrame();
    auto& commandList = WinUtil::GetCommandQueue()->GetCommandList().GetList();

    commandList->OMSetRenderTargets(1, &rtv_handle, true, nullptr);
}

void ImGuiLayer::Render(void* inCommandList)
{
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ID3D12GraphicsCommandList* commandlist = static_cast<ID3D12GraphicsCommandList*>(inCommandList);
    ImGui::Render();
    ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandlist);

    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault(nullptr, inCommandList);
    }
}

void ImGuiLayer::Shutdown()
{
    ImGui_ImplDX12_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}