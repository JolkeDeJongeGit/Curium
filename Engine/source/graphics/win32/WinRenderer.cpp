#include "precomp.h"
#include "graphics/win32/WinDevice.h"
#include "graphics/Renderer.h"

void Renderer::Init()
{
	m_device = &Device::Get();
}

void Renderer::Render()
{
}

void Renderer::Draw(glm::mat4 _model, std::vector<VertexData> _data)
{
}

void Renderer::Draw(glm::mat4 _model, Mesh _data)
{
}

void Renderer::Terminate()
{
	delete m_device;
}
