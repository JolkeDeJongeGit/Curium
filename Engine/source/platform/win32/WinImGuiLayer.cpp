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

void ImGuiLayer::_Init()
{
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.WantCaptureMouse = true;
    io.DisplaySize = ImVec2(static_cast<float>(Engine::GetWindow()->GetWidth()), static_cast<float>(Engine::GetWindow()->GetHeight()));

    float ys;
    float xs;
    glfwGetWindowContentScale(Engine::GetWindow()->GetWindow(), &xs, &ys);
    uiScale = (xs + ys) / 2.0f;

#ifdef _DEBUG
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
#else	
    io.ConfigFlags &= ~ImGuiConfigFlags_NavEnableKeyboard;
#endif

    auto window = Engine::GetWindow()->GetWindow();
    ID3D12DescriptorHeap* srv = WinUtil::GetDescriptorHeap(HeapType::CBV_SRV_UAV)->GetDescriptorHeap().Get();
    ImGui_ImplGlfw_InitForOther(window, true);
    ImGui_ImplDX12_Init(WinUtil::GetDevice()->GetDevice().Get(), Swapchain::BackBufferCount,
        DXGI_FORMAT_R8G8B8A8_UNORM, srv,
        srv->GetCPUDescriptorHandleForHeapStart(),
        srv->GetGPUDescriptorHandleForHeapStart());
}

void ImGuiLayer::NewFrame()
{
    ImGui_ImplDX12_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGuizmo::BeginFrame();
}

void ImGuiLayer::Render(void* inCommandList)
{
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::Render();
    ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), static_cast<ID3D12GraphicsCommandList*>(inCommandList));
}

void ImGuiLayer::Shutdown()
{
    ImGui_ImplDX12_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}