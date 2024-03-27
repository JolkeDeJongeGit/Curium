#pragma once
#include "graphics/win32/WinPipelineState.h"
class PipelineStateScreen : public PipelineState
{
public:
	PipelineStateScreen(const std::string& inVertexName, const std::string& inPixelName, const D3D12_PRIMITIVE_TOPOLOGY_TYPE inType, const bool inUseDepth = true);
	void Render(ComPtr<ID3D12GraphicsCommandList> commandList);

	inline ComPtr<ID3D12PipelineState>& GetPipelineState() { return m_pipelineState; };
	inline ComPtr<ID3D12PipelineState>& GetWireframePipelineState() { return m_wireframePipelineState; };
	inline ComPtr<ID3D12RootSignature>& GetRootSignature() { return m_rootSignature; };
private:
	void SetupRootSignature() ;
	void SetupPipelineState(D3D12_PRIMITIVE_TOPOLOGY_TYPE inType, bool inUseDepth);

	void SetupMesh();

	class Mesh* m_meshScreen = nullptr;

	ComPtr<ID3D12PipelineState> m_pipelineState;
	ComPtr<ID3D12RootSignature> m_rootSignature;

	class Buffer* m_cameraConstant;

	const char* m_vertexName;
	const char* m_pixelName;
};