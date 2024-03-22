#pragma once
#include "graphics/win32/WinPipelineState.h"
class PipelineStateCS : PipelineState
{
private:
	virtual void SetupRootSignature() override;
	virtual void SetupPipelineState(D3D12_PRIMITIVE_TOPOLOGY_TYPE inType, bool inUseDepth) override;
};