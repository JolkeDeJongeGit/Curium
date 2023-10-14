#pragma once
class GameObject;
namespace Debug
{
	void Init();
	void NewFrame();
	void Update(const float inDt);
	void EditProperties(std::unordered_map<std::string, GameObject>& inSceneList);
	void Render();
	bool Paused();
	bool IsWireframeMode();
	void Shutdown();
}