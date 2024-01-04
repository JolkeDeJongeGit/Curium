#pragma once
#include "Components/GameObject.h"

namespace Scene
{
	void Init();
	void Update(const float inDt);
	void AddSceneObject(std::string name, GameObject const& gameobject);
	void HierarchyWindow(bool& inShow);

	GameObject* GetSelectedSceneObject();

	std::unordered_map<std::string, GameObject>& AllSceneObjects();
}
