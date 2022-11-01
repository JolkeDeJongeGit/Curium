#pragma once
class PipelineState
{
public:
	PipelineState(std::string vertexName, std::string pixelName, D3D12_PRIMITIVE_TOPOLOGY_TYPE type, bool useDepth = true);

	ComPtr<ID3D12PipelineState>& GetPipelineState();
	ComPtr<ID3D12RootSignature>& GetRootSignature();
private:
	void SetupRootSignature();
	void SetupPipelineState(D3D12_PRIMITIVE_TOPOLOGY_TYPE type, bool useDepth);

	ComPtr<ID3D12PipelineState> m_pipelineState;
	ComPtr<ID3D12RootSignature> m_rootSignature;

	const char* m_vertexName;
	const char* m_pixelName;
};