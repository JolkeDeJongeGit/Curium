#pragma once
#include "Components/GameObject.h"
#include "include/imgui.h"

namespace Scene
{
	void Init();
	void Update(const float inDt);
	void Shutdown();
	void AddSceneObject(std::string name, GameObject* gameobject);
	void HierarchyWindow(bool& inShow);

	bool StopSubdivide();

	GameObject* GetSelectedSceneObject();

	void SceneGizmo(ImVec2 inPos, ImVec2 inSize);

	std::unordered_map<std::string, GameObject*>& AllSceneObjects();
}
