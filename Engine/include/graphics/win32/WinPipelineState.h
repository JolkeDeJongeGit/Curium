#pragma once
class PipelineState
{
public:
	PipelineState() = default; 
	PipelineState(const std::string& vertexName, const std::string& pixelName, D3D12_PRIMITIVE_TOPOLOGY_TYPE type, bool useDepth = true);

	inline ComPtr<ID3D12PipelineState>& GetPipelineState() { return m_pipelineState; };
	inline ComPtr<ID3D12PipelineState>& GetWireframePipelineState() {return m_wireframePipelineState; };
	inline ComPtr<ID3D12RootSignature>& GetRootSignature() { return m_rootSignature; };
private:
	virtual void SetupRootSignature();
	virtual void SetupPipelineState(D3D12_PRIMITIVE_TOPOLOGY_TYPE inType, bool inUseDepth);

protected:
	ComPtr<ID3D12PipelineState> m_pipelineState;
	ComPtr<ID3D12PipelineState> m_wireframePipelineState;
	ComPtr<ID3D12RootSignature> m_rootSignature;

	const char* m_vertexName;
	const char* m_pixelName;
};