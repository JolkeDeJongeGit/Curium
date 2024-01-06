#pragma once
class GameObject;
namespace Debug
{
	void Init();
	void Update();
	bool& Paused();
	bool& IsWireframeMode();
	void Shutdown();
}