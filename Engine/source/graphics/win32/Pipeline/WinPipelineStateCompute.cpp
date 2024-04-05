#include "precomp.h"
#include "graphics/win32/WinDevice.h"
#include "graphics/ShaderManager.h"
#include <graphics/Mesh.h>
#include "graphics/win32/WinSwapchain.h"
#include "graphics/win32/WinDescriptorHeap.h"
#include "graphics/win32/WinBuffer.h"
#include "graphics/Renderer.h"
#include "graphics/Camera.h"
#include "graphics/win32/Pipeline/WinPipelineStateCompute.h"

PipelineStateCompute::PipelineStateCompute(const std::string& inVertexName)
{
	auto shader = WinUtil::GetShaderManager();
	shader->LoadShader(inVertexName.c_str(), std::string("resources/shaders/").c_str());

	m_computeName = inVertexName.c_str();

	SetupRootSignature();
	SetupPipelineState();
	CreateTexture();
}

void PipelineStateCompute::Render(ComPtr<ID3D12GraphicsCommandList> commandList, int inThreadX, int inThreadY, int inThreadZ)
{
	DescriptorHeap* srvHeap = WinUtil::GetDescriptorHeap(HeapType::CBV_SRV_UAV);

	commandList->SetComputeRootSignature(m_rootSignature.Get());
	commandList->SetPipelineState(m_pipelineState.Get());

	commandList->SetComputeRootDescriptorTable(0, srvHeap->GetGpuHandleAt(6));
	commandList->Dispatch(inThreadX, inThreadX, inThreadZ);
}

void PipelineStateCompute::SetupRootSignature()
{
	const ComPtr<ID3D12Device2> device = WinUtil::GetDevice()->GetDevice();

	D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};
	featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;

	if (const HRESULT result = device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData));  FAILED(result))
		featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;

	// -- Render Texture Descriptor
	CD3DX12_DESCRIPTOR_RANGE1 descRenderRange[1];
	descRenderRange[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0);

	// -- Setup Root Parameters
	CD3DX12_ROOT_PARAMETER1 rootParameter[1];
	rootParameter[0].InitAsDescriptorTable(1, &descRenderRange[0], D3D12_SHADER_VISIBILITY_ALL);

	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init_1_1(_countof(rootParameter), rootParameter, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

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

void PipelineStateCompute::SetupPipelineState()
{
	ComPtr<ID3D12Device2> device = WinUtil::GetDevice()->GetDevice();
	auto shader = WinUtil::GetShaderManager();

	D3D12_COMPUTE_PIPELINE_STATE_DESC pipelineStateStream {};
	pipelineStateStream.pRootSignature = m_rootSignature.Get();
	pipelineStateStream.CS = CD3DX12_SHADER_BYTECODE(shader->GetShader(m_computeName).ShaderBlob.Get());
	pipelineStateStream.NodeMask = 0;
	ThrowIfFailed(device->CreateComputePipelineState(&pipelineStateStream, IID_PPV_ARGS(&m_pipelineState)));
}

void PipelineStateCompute::CreateTexture()
{
	const ComPtr<ID3D12Device2> device = WinUtil::GetDevice()->GetDevice();
	DescriptorHeap* srvHeap = WinUtil::GetDescriptorHeap(HeapType::CBV_SRV_UAV);

	D3D12_RESOURCE_DESC textureDesc = {};
	textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	textureDesc.Width = 2048; // Width of the texture
	textureDesc.Height = 2048; // Height of the texture
	textureDesc.DepthOrArraySize = 1;
	textureDesc.MipLevels = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // Texture format
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	textureDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
	D3D12_HEAP_PROPERTIES properties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	device->CreateCommittedResource(
		&properties,
		D3D12_HEAP_FLAG_NONE,
		&textureDesc,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		nullptr,
		IID_PPV_ARGS(&m_sharedtexture)
	);


	m_sharedIndex = srvHeap->GetNextIndex();

	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	uavDesc.Format = textureDesc.Format;
	uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
	uavDesc.Texture2D.MipSlice = 0;
	uavDesc.Texture2D.PlaneSlice = 0;

	device->CreateUnorderedAccessView(m_sharedtexture.Get(), nullptr, &uavDesc, srvHeap->GetCpuHandleAt(m_sharedIndex));

}
