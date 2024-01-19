#include "precomp.h"
#include "core/Editor.h"
#include "include/imgui.h"
#include "Engine.h"
#include "core/ImGuiLayer.h"
#include "include/imgui_internal.h"
#include "core/Scene.h"
#include "graphics/win32/WinSwapchain.h"
#include "graphics/win32/WinDescriptorHeap.h"
#include "graphics/win32/WinCommandQueue.h"
#include <graphics/Renderer.h>
#include "graphics/Camera.h"
#include "graphics/DebugManager.h"
#include "include/iconfont.h"

namespace Editor
{
	bool show_profiler = true;
	bool show_hierarchy = true;
	bool show_camera = true;

    bool viewport_hovered = false;

    void MainMenu();

    void Viewport();

    void SplitEditor();
}

void Editor::MainMenu()
{
    ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_FirstUseEver);
    ImGui::GetFont()->Scale = 1.5f;
    ImGui::PushFont(ImGui::GetFont());
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Save Scene", "CTRL+S")) {}
            if (ImGui::MenuItem("Load Scene")) {}
            if (ImGui::MenuItem("Exit")) { Engine::Close(); }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Windows"))
        {
            ImGui::MenuItem("Hierarchy", "", &show_hierarchy);
            ImGui::MenuItem("Profiler", "", &show_profiler);
            ImGui::MenuItem("Camera Propertie", "", &show_camera);
            ImGui::Separator();
            ImGui::EndMenu();
        }
        ImGui::MenuItem("Wireframe Mode", "", &Debug::IsWireframeMode());
        ImGui::EndMenuBar();
    }
    ImGui::PopFont();
}

void Editor::Viewport()
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus);

    ImVec2 viewportSize = ImGui::GetContentRegionAvail();
    auto value = WinUtil::GetSwapchain()->m_renderTextureSrvID;
    auto aspectRatio = viewportSize.x / viewportSize.y;
    if((aspectRatio) != Renderer::GetCamera()->GetAspectRatio())
        Renderer::GetCamera()->UpdateProjection(aspectRatio);

    viewport_hovered = ImGui::IsWindowHovered();

    ImGui::Image((ImTextureID)WinUtil::GetDescriptorHeap(HeapType::CBV_SRV_UAV)->GetGpuHandleAt(value).ptr, viewportSize);
    Scene::SceneGizmo(ImGui::GetWindowPos(), viewportSize);

    ImGui::End();
    ImGui::PopStyleVar();
}

void Editor::SplitEditor()
{
    static bool firstTime = true;
    if (firstTime)
    {
        firstTime = false;
        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockBuilderRemoveNode(dockspace_id);
        ImGui::DockBuilderAddNode(dockspace_id);
        ImGui::DockBuilderSetNodeSize(dockspace_id, ImGui::GetMainViewport()->Size);

        auto dock_id_up = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Right, 0.8f, nullptr, &dockspace_id);
        auto dock_id_left = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.2f, nullptr, &dockspace_id);
        auto dock_id_down = ImGui::DockBuilderSplitNode(dock_id_left, ImGuiDir_Down, 0.2f, nullptr, &dock_id_left);
        auto Hierarchy_down = ImGui::DockBuilderSplitNode(dock_id_up, ImGuiDir_Down, 0.2f, nullptr, &dock_id_up);

        if (ImGui::DockBuilderGetNode(ImGui::GetID("Viewport")) == NULL)
        {
            ImGui::DockBuilderDockWindow("Viewport", dock_id_up);
        }

        if (ImGui::DockBuilderGetNode(ImGui::GetID("Hierarchy")) == NULL)
        {
            ImGui::DockBuilderDockWindow("Hierarchy", dock_id_left);
        }

        if (ImGui::DockBuilderGetNode(ImGui::GetID("Camera Properties")) == NULL)
        {
            ImGui::DockBuilderDockWindow("Camera Properties", dock_id_down);
        }

        if (ImGui::DockBuilderGetNode(ImGui::GetID("Profiler")) == NULL)
        {
            ImGui::DockBuilderDockWindow("Profiler", Hierarchy_down);
        }


        ImGui::DockBuilderFinish(dockspace_id);
    }
}

void Editor::Init()
{
}

void Editor::Update()
{
    PROFILE_FUNCTION();
    SplitEditor();
    MainMenu();

    Viewport();

    if (show_profiler)
        Performance::DebugImgui(show_profiler);

    if (show_hierarchy)
        Scene::HierarchyWindow(show_hierarchy);

    if (show_camera)
        Renderer::DrawCameraPropertyWindow();
}

void Editor::Shutdown()
{
    Scene::Shutdown();
}

bool Editor::ViewportHovered()
{
    return viewport_hovered;
}
