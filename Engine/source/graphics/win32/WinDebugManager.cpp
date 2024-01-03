#include "precomp.h"
#include "platform/Window.h"
#include "graphics/win32/WinUtil.h"
#include "platform/win32/WinWindow.h"
#include "graphics/win32/WinDevice.h"
#include "graphics/win32/WinDescriptorHeap.h"
#include "graphics/win32/WinCommandQueue.h"
#include "include/iconfont.h"
#include "include/imgui_internal.h"
#include "include/imgui_impl_dx12.h"
#include <include/imgui_impl_glfw.h>
#include <include/implot.h>
#include "include/ImGuizmo.h"
#include "Components/GameObject.h"
#include "Engine.h"

#include "graphics/win32/WinSwapchain.h"
#include "graphics/Camera.h"
#include "graphics/Renderer.h"
#include "graphics/DebugManager.h"
#include <core/ImGuiLayer.h>

namespace Debug
{
    bool paused = false;
    bool show_profiler = false;
    bool wireframe_mode = false;
    int selected_game_object;

    std::unordered_map<std::string, GameObject> scenelist;

    std::vector<const char*> names;

    ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::TRANSLATE);
    ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::LOCAL);
}


void Debug::Init()
{

}

void Debug::Update(const float inDt)
{
    PROFILE_FUNCTION()

    if (!ImGui::IsMouseDown(ImGuiMouseButton_Right))
    {
        if (ImGui::IsKeyPressed(ImGuiKey_W))
            Debug::mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
        if (ImGui::IsKeyPressed(ImGuiKey_E))
            Debug::mCurrentGizmoOperation = ImGuizmo::ROTATE;
        if (ImGui::IsKeyPressed(ImGuiKey_R)) // r Key
            Debug::mCurrentGizmoOperation = ImGuizmo::SCALE;
    }
}

void Debug::EditProperties(std::unordered_map<std::string, GameObject>& inSceneList)
{
    names.clear();

    for (const auto& [fst, snd] : inSceneList)
        names.push_back(fst.c_str());

    // TODO::Needs to be having a scene selection stuff
    Transform& transform = inSceneList[names[selected_game_object]].GetTransform();
    ImGuiIO& io = ImGui::GetIO();

    ImGuizmo::SetDrawlist(ImGui::GetBackgroundDrawList());
    ImGuizmo::SetRect(ImGui::GetMainViewport()->Pos.x, ImGui::GetMainViewport()->Pos.y, ImGui::GetMainViewport()->Size.x, ImGui::GetMainViewport()->Size.y);
    glm::mat4 model = transform.GetModelMatrix();
    ImGuizmo::Manipulate(glm::value_ptr(Renderer::GetCamera()->GetView()), glm::value_ptr(Renderer::GetCamera()->GetProjection()), mCurrentGizmoOperation, mCurrentGizmoMode, glm::value_ptr(model), nullptr);

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

bool Debug::Paused()
{
    bool t = paused;
    return t;
}

bool Debug::IsWireframeMode()
{
    return wireframe_mode;
}

void Debug::Shutdown()
{

}