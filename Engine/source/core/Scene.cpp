#include "precomp.h"
#include "core/Scene.h"
#include "include/ImGuizmo.h"
#include "graphics/DebugManager.h"
#include "graphics/Renderer.h"
#include "components/terrain/TerrainQuadTree.h"
#include "components/gameobjects/Planet.h"
#include "graphics/win32/WinCommandQueue.h"

namespace Scene
{
    std::unordered_map<std::string, GameObject*> gameobjects;

    GameObject* selected_game_object = nullptr;
    bool stop_subdivide = false;
    ImGuizmo::OPERATION m_current_gizmo_operation(ImGuizmo::TRANSLATE);
    ImGuizmo::MODE current_gizmo_mode(ImGuizmo::LOCAL);
}

void Scene::Init()
{
    auto commandqueue = WinUtil::GetCommandQueue();
    commandqueue->OpenCommandList();

    for (auto& [name, gameobject] : gameobjects)
        gameobject->Init();

    commandqueue->CloseCommandList();
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
    ImGui::Checkbox("Stop subdividing", &stop_subdivide);
    ImGui::End();
    ImGui::PopStyleVar();
}

void Scene::PropertyWindow(bool& inShow)
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 10));
    ImGui::Begin("Property Window");
    Planet* gameObject = static_cast<Planet*>(Scene::AllSceneObjects()["World"]);
    ImGui::LabelText("Planet", "");
    ImGui::SliderFloat("Atmosphere Radius", &gameObject->m_atmosphereRadius, gameObject->m_planetRadius, 130000 + 15000);
    ImGui::SliderFloat("Fall off Radius", &gameObject->m_fallOff, -100.f, 100.f);

    ImGui::End();
    ImGui::PopStyleVar();
}

bool Scene::StopSubdivide()
{
    return stop_subdivide;
}

void Scene::SceneGizmo(ImVec2 inPos, ImVec2 inSize)
{
    ImGuiIO& io = ImGui::GetIO();
    for (auto& [name, gameobject] : gameobjects)
    {
        gameobject->Update();
    }

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
