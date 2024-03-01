#include "precomp.h"
#include "core/Scene.h"
#include "include/ImGuizmo.h"
#include "graphics/DebugManager.h"
#include "graphics/Renderer.h"
#include "graphics/Camera.h"
#include <components/terrain/TerrainQuadTree.h>

namespace Scene
{
    std::unordered_map<std::string, GameObject*> gameobjects;

    GameObject* selected_game_object = nullptr;

    ImGuizmo::OPERATION m_current_gizmo_operation(ImGuizmo::TRANSLATE);
    ImGuizmo::MODE current_gizmo_mode(ImGuizmo::LOCAL);

    TerrainQuadTree tree;
}

void Scene::Init()
{
    auto world = gameobjects["World"];
    tree = TerrainQuadTree(dynamic_cast<Terrain*>(world));
    tree.Init();

    for (auto& [name, gameobject] : gameobjects)
        gameobject->Init();
}

void Scene::Update(const float inDt)
{
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
        gameobject->Update();
    }
}

void Scene::Shutdown()
{
    for (auto obj : gameobjects)
    {
        delete obj.second;
    }
    gameobjects.clear();
}

void Scene::AddSceneObject(std::string name, GameObject* gameobject)
{
    gameobjects.insert({ name, gameobject });
}

void Scene::HierarchyWindow(bool& inShow)
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 10));
    ImGui::Begin("Hierarchy");
    ImGui::PushID(1);
    ImGui::PushItemWidth(-1);
    std::vector<const char*> itemNames;
    itemNames.reserve(gameobjects.size());

    int selectedId = -1;
    int currentId = 0;
    for (const auto& obj : gameobjects)
    {
        const std::string& name = obj.first;

        if (selected_game_object == obj.second)
            selectedId = currentId;

        itemNames.push_back(obj.first.c_str());

        currentId++;
    }
    ImGui::ListBox("", &selectedId, itemNames.data(), static_cast<int>(itemNames.size()));
    if (selectedId >= 0)
        selected_game_object = gameobjects[itemNames[selectedId]];

    ImGui::PopItemWidth();
    ImGui::PopID();
    ImGui::Checkbox("Stop subdividing", &tree.m_stopSubdivide);
    ImGui::End();
    ImGui::PopStyleVar();
}

void Scene::SceneGizmo(ImVec2 inPos, ImVec2 inSize)
{
    tree.Update();

    ImGuiIO& io = ImGui::GetIO();
    if (GameObject* gameobject = GetSelectedSceneObject())
    {
        ImGuizmo::BeginFrame();
        Transform transform = gameobject->GetTransform();
        ImGuizmo::SetDrawlist();
        ImGuizmo::SetRect(inPos.x, inPos.y, inSize.x, inSize.y);
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

            gameobject->SetTransform(transform);
        }
    }
}

GameObject* Scene::GetSelectedSceneObject()
{
    return selected_game_object;
}

std::unordered_map<std::string, GameObject*>& Scene::AllSceneObjects()
{
    return gameobjects;
}
