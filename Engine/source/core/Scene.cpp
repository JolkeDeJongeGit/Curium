#include "precomp.h"
#include "core/Scene.h"
#include "include/imgui.h"
#include "include/ImGuizmo.h"
#include "graphics/DebugManager.h"
#include "graphics/Renderer.h"
#include "graphics/Camera.h"

namespace Scene
{
    std::unordered_map<std::string, GameObject> gameobjects;
    std::vector<const char*> names;

    int selected_game_object;

    ImGuizmo::OPERATION m_current_gizmo_operation(ImGuizmo::TRANSLATE);
    ImGuizmo::MODE current_gizmo_mode(ImGuizmo::LOCAL);

    void SceneGizmo();
}

void Scene::Init()
{
    for (auto& [name, gameobject] : gameobjects)
        gameobject.Init();
}

void Scene::Update(const float inDt)
{
    names.clear();

    if (!ImGui::IsMouseDown(ImGuiMouseButton_Right))
    {
        if (ImGui::IsKeyPressed(ImGuiKey_W))
            m_current_gizmo_operation = ImGuizmo::TRANSLATE;
        if (ImGui::IsKeyPressed(ImGuiKey_E))
            m_current_gizmo_operation = ImGuizmo::ROTATE;
        if (ImGui::IsKeyPressed(ImGuiKey_R)) // r Key
            m_current_gizmo_operation = ImGuizmo::SCALE;
    }

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
    SceneGizmo();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 10));
    ImGui::Begin("Hierarchy");
    ImGui::PushID(1);
    ImGui::PushItemWidth(-1);
    ImGui::ListBox("", &selected_game_object, names.data(), static_cast<int>(names.size()), static_cast<int>(names.size()));
    ImGui::PopItemWidth();
    ImGui::PopID();
    ImGui::End();
    ImGui::PopStyleVar();
}

void  Scene::SceneGizmo()
{
    ImGuiIO& io = ImGui::GetIO();
    if (GameObject* gameobject = GetSelectedSceneObject())
    {
        Transform& transform = gameobject->GetTransform();
        ImGuizmo::SetDrawlist(ImGui::GetBackgroundDrawList());
        ImGuizmo::SetRect(ImGui::GetMainViewport()->Pos.x, ImGui::GetMainViewport()->Pos.y, ImGui::GetMainViewport()->Size.x, ImGui::GetMainViewport()->Size.y);
        glm::mat4 model = transform.GetModelMatrix();
        ImGuizmo::Manipulate(glm::value_ptr(Renderer::GetCamera()->GetView()), glm::value_ptr(Renderer::GetCamera()->GetProjection()), m_current_gizmo_operation, current_gizmo_mode, glm::value_ptr(model), nullptr);

        if (ImGuizmo::IsUsing())
        {
            glm::vec3 translation, scale, rotation;
            DecomposeTransform(model, translation, scale, rotation);
            glm::vec3 deltaRotation = rotation - transform.GetEulerRotation();
            transform.SetPosition(translation);
            transform.SetScale(scale);
            transform.Rotate(deltaRotation);
        }
    }
}

GameObject* Scene::GetSelectedSceneObject()
{
    if (names.size() > selected_game_object)
    {
        std::string name = names[selected_game_object];
        if(gameobjects.find(name) != gameobjects.end())
            return &gameobjects[name];
    }
    return nullptr;
}

std::unordered_map<std::string, GameObject>& Scene::AllSceneObjects()
{
    return gameobjects;
}
