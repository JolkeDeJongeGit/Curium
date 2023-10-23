#pragma once
class GameObject;
namespace Debug
{
	void Init();
	void NewFrame();
	void Update(const float inDt);
	void EditProperties(std::unordered_map<std::string, GameObject>& inSceneList);
	void Render(ComPtr<ID3D12GraphicsCommandList> const& inCommandList);
	bool Paused();
	bool IsWireframeMode();
	void Shutdown();
}