#include "precomp.h"
#include "core/Scene.h"
#include "include/imgui.h"
#include <graphics/DebugManager.h>

namespace Scene
{
    std::unordered_map<std::string, GameObject> gameobjects;
    std::vector<const char*> names;

    int selected_game_object;
}

void Scene::Init()
{
    for (auto& [name, gameobject] : gameobjects)
        gameobject.Init();
}

void Scene::Update(const float inDt)
{
    names.clear();

    for (auto& [name, gameobject] : gameobjects)
    {
        names.push_back(name.c_str());
        gameobject.Update();
    }
}

void Scene::AddSceneObject(std::string name, GameObject const& gameobject)
{
    gameobjects.insert(std::pair<std::string, GameObject>(name, gameobject));
}

void Scene::HierarchyWindow(bool& inShow)
{
    Debug::EditProperties(gameobjects);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 10));
    ImGui::Begin("Hierarchy");
    ImGui::PushID(1);
    ImGui::PushItemWidth(-1);
    ImGui::ListBox("", &selected_game_object, names.data(), static_cast<int>(names.size()), static_cast<int>(names.size()));
    ImGui::PopItemWidth();
    ImGui::PopID();
    ImGui::End();
    ImGui::PopStyleVar(2);
}

GameObject& Scene::GetSelectedSceneObject()
{
    return gameobjects[names[selected_game_object]];
}

std::unordered_map<std::string, GameObject>& Scene::AllSceneObjects()
{
    return gameobjects;
}
