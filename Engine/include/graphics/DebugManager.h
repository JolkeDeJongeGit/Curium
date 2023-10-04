#pragma once
class GameObject;
namespace Debug
{
	void Init();
	void Update(const float inDt);
	void EditProperties(std::unordered_map<std::string, GameObject>& inSceneList);
	void Render();
	bool Paused();
	void Shutdown();
}