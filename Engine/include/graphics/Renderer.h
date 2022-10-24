#pragma once
#include "Mesh.h"
class Renderer
{
public:
	void Init();
	void Render();

	void Draw(glm::mat4 _model, std::vector<VertexData> _data);
	void Draw(glm::mat4 _model, Mesh _data);

	//void InstanceDraw();
private:
};