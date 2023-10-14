#pragma once
class PipelineState
{
public:
	PipelineState(const std::string& vertexName, const std::string& pixelName, D3D12_PRIMITIVE_TOPOLOGY_TYPE type, bool useDepth = true);

	ComPtr<ID3D12PipelineState>& GetPipelineState();
	ComPtr<ID3D12PipelineState>& GetWireframePipelineState();
	ComPtr<ID3D12RootSignature>& GetRootSignature();
private:
	void SetupRootSignature();
	void SetupPipelineState(D3D12_PRIMITIVE_TOPOLOGY_TYPE inType, bool inUseDepth);

	ComPtr<ID3D12PipelineState> m_pipelineState;
	ComPtr<ID3D12PipelineState> m_wireframePipelineState;
	ComPtr<ID3D12RootSignature> m_rootSignature;

	const char* m_vertexName;
	const char* m_pixelName;
};