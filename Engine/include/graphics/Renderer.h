#pragma once
#include "Mesh.h"
class Camera;
namespace Renderer
{
	void Init(uint32_t inWidth, uint32_t  inHeight);
	void Render();
	void Update();
	void Shutdown();
	void Resize(uint32_t inWidth, uint32_t  inHeight);
	void DrawCameraPropertyWindow();
	void AddRemovedMesh(Mesh mesh);

	Camera* GetCamera();
}
