#pragma once
#include "graphics/win32/WinPipelineState.h"
class PipelineStateCompute : public PipelineState
{
public:
	PipelineStateCompute(const std::string& inComputeShader);

	void Render(ComPtr<ID3D12GraphicsCommandList> commandList, int inThreadX, int inThreadY, int inThreadZ);

	inline ComPtr<ID3D12PipelineState>& GetPipelineState() { return m_pipelineState; };
	inline ComPtr<ID3D12RootSignature>& GetRootSignature() { return m_rootSignature; };

	int m_sharedIndex;
private:
	void SetupRootSignature();
	void SetupPipelineState();

	void CreateTexture();

	ComPtr<ID3D12Resource> m_sharedtexture;
	const char* m_computeName;
};