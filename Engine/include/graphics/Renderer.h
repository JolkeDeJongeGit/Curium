#pragma once
#include "Mesh.h"

class Device;
class CommandQueue;
class Swapchain;
class PipelineState;
class DescriptorHeap;

class Renderer
{
public:
	void Init(uint32_t _width, uint32_t  _height);
	void Render();

	void Draw();
	void Draw(glm::mat4 _model, std::vector<VertexData> _data);
	void Draw(glm::mat4 _model, Mesh _data);

	//void InstanceDraw();

	void Terminate();
private:
	Device* m_device = nullptr;
	CommandQueue* m_commandQueue = nullptr;
	Swapchain* m_swapchain = nullptr;
	PipelineState* m_pipelineState = nullptr;

	DescriptorHeap* m_cbvHeap;
	DescriptorHeap* m_rtvHeap;
	DescriptorHeap* m_dsvHeap;

	uint32_t m_viewportWidth = 1920;
	uint32_t m_viewportHeight = 1080;

	uint32_t m_frames;
};