#pragma once
#include "Components/GameObject.h"
#include "include/imgui.h"

namespace Scene
{
	void Init();
	void Update(const float inDt);
	void AddSceneObject(std::string name, GameObject const& gameobject);
	void HierarchyWindow(bool& inShow);

	GameObject* GetSelectedSceneObject();

	void SceneGizmo(ImVec2 inPos, ImVec2 inSize);

	std::unordered_map<std::string, GameObject>& AllSceneObjects();
}
