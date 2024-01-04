#include "precomp.h"
#include "core/Editor.h"
#include "include/imgui.h"
#include "Engine.h"
#include "core/ImGuiLayer.h"
#include "include/imgui_internal.h"
#include "core/Scene.h"


namespace Editor
{
	bool show_profiler = true;
	bool show_hierarchy = true;
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
            ImGui::Separator();
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
    ImGui::PopFont();
}

void Editor::Viewport()
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin("Viewport");
    ImVec2 viewportSize = ImGui::GetContentRegionAvail();

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

        auto dock_id_up = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Up, 0.8f, nullptr, &dockspace_id);
        auto dock_id_down = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Down, 0.2f, nullptr, &dockspace_id);
        auto dock_id_left = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.5f, nullptr, &dockspace_id);

        if (ImGui::DockBuilderGetNode(ImGui::GetID("Hierarchy")) == NULL)
        {
            ImGui::DockBuilderDockWindow("Hierarchy", dock_id_left);
        }

        if (ImGui::DockBuilderGetNode(ImGui::GetID("Viewport")) == NULL)
        {
            ImGui::DockBuilderDockWindow("Viewport", dock_id_up);
        }

        if (ImGui::DockBuilderGetNode(ImGui::GetID("Profiler")) == NULL)
        {
            ImGui::DockBuilderDockWindow("Profiler", dock_id_down);
        }


        ImGui::DockBuilderFinish(dockspace_id);
    }
}

void Editor::Init()
{
}

void Editor::Update(const float inDt)
{
    PROFILE_FUNCTION();

    MainMenu();

    Viewport();

    if (show_profiler)
        Performance::DebugImgui(show_profiler);

    if (show_hierarchy)
        Scene::HierarchyWindow(show_hierarchy);
}

void Editor::Shutdown()
{
}