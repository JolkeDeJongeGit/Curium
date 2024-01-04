#include "precomp.h"
#include "core/Editor.h"
#include <include/imgui.h>
#include <Engine.h>


namespace Editor
{
	bool show_profiler = true;
    void MainMenu();
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
            if (ImGui::MenuItem("Hierarchy")) {}
            ImGui::MenuItem("Profiler", "", &show_profiler);
            ImGui::Separator();
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
    ImGui::PopFont();
}

void Editor::Init()
{
}

void Editor::Update(const float inDt)
{
    PROFILE_FUNCTION();

    MainMenu();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin("Viewport");
    ImVec2 viewportSize = ImGui::GetContentRegionAvail();
    ImGui::End();
    ImGui::PopStyleVar();

    if (show_profiler)
        Performance::DebugImgui(show_profiler);
}

void Editor::Shutdown()
{
}
