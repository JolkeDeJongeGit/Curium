#pragma once
namespace Debug
{
	void Init();
	void Update(const float inDt);
	void Render();
	bool Paused();
	void Shutdown();
}