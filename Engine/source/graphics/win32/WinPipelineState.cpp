#include "precomp.h"
#include <graphics/ShaderManager.h>
#include <graphics/win32/WinDevice.h>
#include "graphics/win32/WinPipelineState.h"


struct PipelineStateStream
{
	CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE pRootSignature;
	CD3DX12_PIPELINE_STATE_STREAM_INPUT_LAYOUT InputLayout;
	CD3DX12_PIPELINE_STATE_STREAM_PRIMITIVE_TOPOLOGY PrimitiveTopologyType;
	CD3DX12_PIPELINE_STATE_STREAM_VS VS;
	CD3DX12_PIPELINE_STATE_STREAM_PS PS;
	CD3DX12_PIPELINE_STATE_STREAM_HS HS;
	CD3DX12_PIPELINE_STATE_STREAM_DS DS;
	CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL DepthStencil;
	CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL_FORMAT DSVFormat;
	CD3DX12_PIPELINE_STATE_STREAM_RENDER_TARGET_FORMATS RTVFormats;
	CD3DX12_PIPELINE_STATE_STREAM_RASTERIZER Rasterizer;
};

PipelineState::PipelineState(const std::string& inVertexName, const std::string& inPixelName, const D3D12_PRIMITIVE_TOPOLOGY_TYPE inType, const bool inUseDepth)
{
	auto& shader = ShaderManager::Get();
	shader.Init();
	shader.LoadShader(inVertexName.c_str(), std::string("resources/shaders/").c_str());
	shader.LoadShader(inPixelName.c_str(), std::string("resources/shaders/").c_str());
	shader.LoadShader("basic.hull", std::string("resources/shaders/").c_str());
	shader.LoadShader("basic.domain", std::string("resources/shaders/").c_str());

	m_vertexName = inVertexName.c_str();
	m_pixelName = inPixelName.c_str();

	SetupRootSignature();
	SetupPipelineState(inType, inUseDepth);
}

ComPtr<ID3D12PipelineState>& PipelineState::GetPipelineState()
{
	return m_pipelineState;
}

ComPtr<ID3D12PipelineState>& PipelineState::GetWireframePipelineState()
{
	return m_wireframePipelineState;
}

ComPtr<ID3D12RootSignature>& PipelineState::GetRootSignature()
{
	return m_rootSignature;
}

void PipelineState::SetupRootSignature()
{
	const ComPtr<ID3D12Device2> device =  WinUtil::GetDevice()->GetDevice();

	D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};
	featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;

	if (const HRESULT result = device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData));  FAILED(result))
		featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;

	CD3DX12_DESCRIPTOR_RANGE1 descRange[1];
	descRange[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

	CD3DX12_ROOT_PARAMETER1 hsTessFactorsCb;
	ZeroMemory(&hsTessFactorsCb, sizeof(hsTessFactorsCb));
	hsTessFactorsCb.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	hsTessFactorsCb.Constants = { 0, 0, 2 }; // 2 constants in first register (b0) in first register space
	hsTessFactorsCb.ShaderVisibility = D3D12_SHADER_VISIBILITY_HULL; // only used in hull shader

	CD3DX12_ROOT_PARAMETER1 dsObjCb;
	ZeroMemory(&dsObjCb, sizeof(dsObjCb));
	dsObjCb.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; // constant buffer
	dsObjCb.Descriptor = { 0, 1 }; // first register (b0) in first register space
	dsObjCb.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; // only used in domain shader

	CD3DX12_ROOT_PARAMETER1 rootParameter[3];
	rootParameter[0] = dsObjCb;
	rootParameter[1] = hsTessFactorsCb;
	rootParameter[2].InitAsDescriptorTable(1, &descRange[0]);

	CD3DX12_STATIC_SAMPLER_DESC	descSamplers[1];
	descSamplers[0].Init(0, D3D12_FILTER_MIN_MAG_MIP_LINEAR);
	descSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	descSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	descSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;

	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init_1_1(_countof(rootParameter), rootParameter, 1, &descSamplers[0], D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> signatureBlob;
	ComPtr<ID3DBlob> errorBlob;

	ThrowIfFailed(D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, featureData.HighestVersion, &signatureBlob, &errorBlob));
	ThrowIfFailed(device->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature)));

	if (errorBlob)
	{
		const auto errStr = static_cast<const char*>(errorBlob->GetBufferPointer());
		printf("%s", errStr);
	}
}

void PipelineState::SetupPipelineState(D3D12_PRIMITIVE_TOPOLOGY_TYPE inType, bool inUseDepth)
{
	ComPtr<ID3D12Device2> device =  WinUtil::GetDevice()->GetDevice();
	auto& shader = ShaderManager::Get();
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
	};

	PipelineStateStream pipelineStateStream;

	D3D12_RT_FORMAT_ARRAY rtvFormats = {};
	rtvFormats.NumRenderTargets = 3;
	rtvFormats.RTFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	rtvFormats.RTFormats[1] = DXGI_FORMAT_R8G8B8A8_UNORM;
	rtvFormats.RTFormats[2] = DXGI_FORMAT_R8G8B8A8_UNORM;

	CD3DX12_DEPTH_STENCIL_DESC depthStencilDesc{ CD3DX12_DEFAULT() };
	depthStencilDesc.DepthEnable = inUseDepth;
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	CD3DX12_RASTERIZER_DESC rasterizer{ CD3DX12_DEFAULT() };
	rasterizer.CullMode = D3D12_CULL_MODE_BACK;

	pipelineStateStream.pRootSignature = m_rootSignature.Get();
	pipelineStateStream.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
	pipelineStateStream.PrimitiveTopologyType = inType;
	pipelineStateStream.VS = CD3DX12_SHADER_BYTECODE(shader.GetShader(m_vertexName).ShaderBlob.Get());
	pipelineStateStream.PS = CD3DX12_SHADER_BYTECODE(shader.GetShader(m_pixelName).ShaderBlob.Get());
	pipelineStateStream.HS = CD3DX12_SHADER_BYTECODE(shader.GetShader("basic.hull").ShaderBlob.Get());
	pipelineStateStream.DS = CD3DX12_SHADER_BYTECODE(shader.GetShader("basic.domain").ShaderBlob.Get());
	pipelineStateStream.DepthStencil = depthStencilDesc;
	pipelineStateStream.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	pipelineStateStream.RTVFormats = rtvFormats;
	pipelineStateStream.Rasterizer = rasterizer;

	D3D12_PIPELINE_STATE_STREAM_DESC pipelineStateStreamDesc = {
		sizeof(PipelineStateStream), &pipelineStateStream
	};

	ThrowIfFailed(device->CreatePipelineState(&pipelineStateStreamDesc, IID_PPV_ARGS(&m_pipelineState)));

	// Create wireframe version
	CD3DX12_RASTERIZER_DESC rasterizer1{ CD3DX12_DEFAULT() };
	rasterizer1.FillMode = D3D12_FILL_MODE_WIREFRAME;
	rasterizer1.CullMode = D3D12_CULL_MODE_NONE;

	PipelineStateStream wirePipelineStateStream;

	wirePipelineStateStream.pRootSignature = m_rootSignature.Get();
	wirePipelineStateStream.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
	wirePipelineStateStream.PrimitiveTopologyType = inType;
	wirePipelineStateStream.VS = CD3DX12_SHADER_BYTECODE(shader.GetShader(m_vertexName).ShaderBlob.Get());
	wirePipelineStateStream.PS = CD3DX12_SHADER_BYTECODE(shader.GetShader(m_pixelName).ShaderBlob.Get());
	wirePipelineStateStream.HS = CD3DX12_SHADER_BYTECODE(shader.GetShader("basic.hull").ShaderBlob.Get());
	wirePipelineStateStream.DS = CD3DX12_SHADER_BYTECODE(shader.GetShader("basic.domain").ShaderBlob.Get());
	wirePipelineStateStream.DepthStencil = depthStencilDesc;
	wirePipelineStateStream.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	wirePipelineStateStream.RTVFormats = rtvFormats;
	wirePipelineStateStream.Rasterizer = rasterizer1;

	D3D12_PIPELINE_STATE_STREAM_DESC pipelineStateStreamDesc1 = {
		sizeof(PipelineStateStream), &wirePipelineStateStream
	};

	ThrowIfFailed(device->CreatePipelineState(&pipelineStateStreamDesc1, IID_PPV_ARGS(&m_wireframePipelineState)));
}
