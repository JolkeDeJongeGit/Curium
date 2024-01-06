#pragma once
class GameObject;
namespace Debug
{
	void Init();
	void Update(const float inDt);
	bool& Paused();
	bool& IsWireframeMode();
	void Shutdown();
}