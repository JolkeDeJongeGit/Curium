#include "precomp.h"
#include "graphics/win32/WinDevice.h"
#include "graphics/ShaderManager.h"
#include "graphics/win32/Pipeline/WinPipelineStateScreen.h"
#include <graphics/Mesh.h>
#include "graphics/win32/WinSwapchain.h"
#include "graphics/win32/WinDescriptorHeap.h"
#include "graphics/win32/WinBuffer.h"
#include "graphics/Renderer.h"
#include "graphics/Camera.h"
#include <core/Scene.h>
#include <components/gameobjects/Planet.h>

PipelineStateScreen::PipelineStateScreen(const std::string& inVertexName, const std::string& inPixelName, const D3D12_PRIMITIVE_TOPOLOGY_TYPE inType, const bool inUseDepth)
{
	auto shader = WinUtil::GetShaderManager();
	shader->LoadShader(inVertexName.c_str(), std::string("resources/shaders/").c_str());
	shader->LoadShader(inPixelName.c_str(), std::string("resources/shaders/").c_str());

	m_vertexName = inVertexName.c_str();
	m_pixelName = inPixelName.c_str();

	SetupRootSignature();
	SetupPipelineState(inType, inUseDepth);

	SetupMesh();

	// TODO Convert this to non hard coded values
	m_cameraConstant = new Buffer();
	m_cameraConstant->CreateConstantBuffer(37 * sizeof(float));

	m_planetConstant = new Buffer();
	m_planetConstant->CreateConstantBuffer(6 * sizeof(float));

	m_sunConstant = new Buffer();
	m_sunConstant->CreateConstantBuffer(8 * sizeof(float));
}

void PipelineStateScreen::Render(ComPtr<ID3D12GraphicsCommandList> commandList)
{
	DescriptorHeap* dsvHeap = WinUtil::GetDescriptorHeap(HeapType::DSV);
	DescriptorHeap* srvHeap = WinUtil::GetDescriptorHeap(HeapType::CBV_SRV_UAV);
	DescriptorHeap* rtvHeap = WinUtil::GetDescriptorHeap(HeapType::RTV);

	ID3D12Resource* renderTarget = WinUtil::GetSwapchain()->GetRenderTextureBuffer().Get();
	ID3D12Resource* depthTarget = WinUtil::GetSwapchain()->GetDepthBuffer().Get();

	uint32_t renderTextureID = WinUtil::GetSwapchain()->m_renderTextureSrvID;
	uint32_t depthTextureID = WinUtil::GetSwapchain()->m_depthTextureSrvID;
	uint32_t renderTextureHeapIndex = WinUtil::GetSwapchain()->m_renderTextureHeapID;

	// -- Gather the GPU/CPU Handle(s)
	CD3DX12_GPU_DESCRIPTOR_HANDLE renderTexture = srvHeap->GetGpuHandleAt(renderTextureID);
	CD3DX12_GPU_DESCRIPTOR_HANDLE depthTexture = srvHeap->GetGpuHandleAt(depthTextureID);
	CD3DX12_CPU_DESCRIPTOR_HANDLE depthView = dsvHeap->GetCpuHandleAt(0);

	// -- Set Pipeline State
	commandList->SetGraphicsRootSignature(m_rootSignature.Get());
	commandList->SetPipelineState(m_pipelineState.Get());

	const CD3DX12_RESOURCE_BARRIER depthTargetBarrier = CD3DX12_RESOURCE_BARRIER::Transition(depthTarget, D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	commandList->ResourceBarrier(1, &depthTargetBarrier);

	const CD3DX12_RESOURCE_BARRIER RenderTargetBarrier = CD3DX12_RESOURCE_BARRIER::Transition(renderTarget, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	commandList->ResourceBarrier(1, &RenderTargetBarrier);

	// -- Set Root Descriptor Table
	commandList->SetGraphicsRootDescriptorTable(0, renderTexture);
	commandList->SetGraphicsRootDescriptorTable(1, depthTexture);
	
	// -- World position Camera
	Camera* camera = Renderer::GetCamera();

	struct camData
	{
		glm::mat4 viewMatrix;
		glm::mat4 inverseviewMatrix;
		glm::vec3 worldCam;
		float _near;
		float _far;
	};
	camData cameraData{ glm::transpose(camera->GetView()), glm::inverse(camera->GetProjection()), camera->GetTransform().GetPosition() };

	// -- Update Camera data
	m_cameraConstant->UpdateBuffer(&cameraData);
	m_cameraConstant->SetGraphicsRootConstantBufferView(commandList, 2);

	struct PlanetData
	{
		glm::vec3 position;
		float planetRadius;
		float atmosphereRadius;
		float atmosphereFalloff;
	};

	// I love Hard coding :(
	Planet* gameObject = static_cast<Planet*>(Scene::AllSceneObjects()["World"]);
	PlanetData worldData { gameObject->GetTransform().GetPosition(), gameObject->m_planetRadius, gameObject->m_atmosphereRadius, gameObject->m_fallOff };

	// -- Update Camera data
	m_planetConstant->UpdateBuffer(&worldData);
	m_planetConstant->SetGraphicsRootConstantBufferView(commandList, 3);

	struct Light
	{
		glm::vec3 position;
		glm::vec3 direction;
		float color;
		float intensity;
	};
	GameObject* light = Scene::AllSceneObjects()["Sun"];
	Light lightData{ light->GetTransform().GetPosition(), light->GetTransform().GetForwardVector(), 0xFFFFFF, 1.f};

	// -- Update Camera data
	m_sunConstant->UpdateBuffer(&lightData);
	m_sunConstant->SetGraphicsRootConstantBufferView(commandList, 4);

	const CD3DX12_RESOURCE_BARRIER RenderTargetBarrierRT = CD3DX12_RESOURCE_BARRIER::Transition(renderTarget, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,  D3D12_RESOURCE_STATE_RENDER_TARGET );
	commandList->ResourceBarrier(1, &RenderTargetBarrierRT);

	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// -- Draw Screen Quad
	commandList->IASetVertexBuffers(0, 1, &m_meshScreen->GetVertexView());
	commandList->IASetIndexBuffer(&m_meshScreen->GetIndexView());
	commandList->DrawIndexedInstanced(static_cast<UINT>(m_meshScreen->m_indexData.size()), 1, 0, 0, 0);

	const CD3DX12_RESOURCE_BARRIER depthTargetBarrierWrite = CD3DX12_RESOURCE_BARRIER::Transition(depthTarget, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_DEPTH_WRITE);
	commandList->ResourceBarrier(1, &depthTargetBarrierWrite);
}

void PipelineStateScreen::SetupRootSignature()
{
	const ComPtr<ID3D12Device2> device = WinUtil::GetDevice()->GetDevice();

	D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};
	featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;

	if (const HRESULT result = device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData));  FAILED(result))
		featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;

	// -- Render Texture Descriptor
	CD3DX12_DESCRIPTOR_RANGE1 descRenderRange[1];
	descRenderRange[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

	// -- Depth Texture Descriptor
	CD3DX12_DESCRIPTOR_RANGE1 descDepthRange[1];
	descDepthRange[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);

	// -- Constant Buffer for Camera Data
	CD3DX12_ROOT_PARAMETER1 constantBufferCam;
	ZeroMemory(&constantBufferCam, sizeof(constantBufferCam));
	constantBufferCam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; // constant buffer
	constantBufferCam.Descriptor = { 0, 0 }; // first register (b0) in first register space
	constantBufferCam.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // only used in _PIXEL shader

	CD3DX12_ROOT_PARAMETER1 constantPlanet;
	ZeroMemory(&constantPlanet, sizeof(constantPlanet));
	constantPlanet.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; // constant buffer
	constantPlanet.Descriptor = { 1, 0 }; // first register (b1) in first register space
	constantPlanet.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // only used in _PIXEL shader

	CD3DX12_ROOT_PARAMETER1 constantLight;
	ZeroMemory(&constantLight, sizeof(constantLight));
	constantLight.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; // constant buffer
	constantLight.Descriptor = { 2, 0 }; // first register (b2) in first register space
	constantLight.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // only used in _PIXEL shader

	// -- Setup Root Parameters
	CD3DX12_ROOT_PARAMETER1 rootParameter[5];
	rootParameter[0].InitAsDescriptorTable(1, &descRenderRange[0], D3D12_SHADER_VISIBILITY_PIXEL);
	rootParameter[1].InitAsDescriptorTable(1, &descDepthRange[0], D3D12_SHADER_VISIBILITY_PIXEL);
	rootParameter[2] = constantBufferCam;
	rootParameter[3] = constantPlanet;
	rootParameter[4] = constantLight;
	
	// -- Setup Texture Sampler
	CD3DX12_STATIC_SAMPLER_DESC	descSamplers[1];
	descSamplers[0].Init(0, D3D12_FILTER_MIN_MAG_MIP_LINEAR);
	descSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	descSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	descSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;

	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init_1_1(_countof(rootParameter), rootParameter, 1, &descSamplers[0], D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

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

void PipelineStateScreen::SetupPipelineState(D3D12_PRIMITIVE_TOPOLOGY_TYPE inType, bool inUseDepth)
{
	ComPtr<ID3D12Device2> device = WinUtil::GetDevice()->GetDevice();
	auto shader = WinUtil::GetShaderManager();
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
	};

	struct PipelineStateStream
	{
		CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE pRootSignature;
		CD3DX12_PIPELINE_STATE_STREAM_INPUT_LAYOUT InputLayout;
		CD3DX12_PIPELINE_STATE_STREAM_PRIMITIVE_TOPOLOGY PrimitiveTopologyType;
		CD3DX12_PIPELINE_STATE_STREAM_VS VS;
		CD3DX12_PIPELINE_STATE_STREAM_PS PS;
		CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL DepthStencil;
		CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL_FORMAT DSVFormat;
		CD3DX12_PIPELINE_STATE_STREAM_RENDER_TARGET_FORMATS RTVFormats;
		CD3DX12_PIPELINE_STATE_STREAM_RASTERIZER Rasterizer;
	};

	PipelineStateStream pipelineStateStream;

	D3D12_RT_FORMAT_ARRAY rtvFormats = {};
	rtvFormats.NumRenderTargets = 3;
	rtvFormats.RTFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	rtvFormats.RTFormats[1] = DXGI_FORMAT_R8G8B8A8_UNORM;
	rtvFormats.RTFormats[2] = DXGI_FORMAT_R8G8B8A8_UNORM;

	CD3DX12_DEPTH_STENCIL_DESC depthStencilDesc{ CD3DX12_DEFAULT() };
	depthStencilDesc.DepthEnable = false;
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	CD3DX12_RASTERIZER_DESC rasterizer{ CD3DX12_DEFAULT() };
	rasterizer.CullMode = D3D12_CULL_MODE_BACK;

	pipelineStateStream.pRootSignature = m_rootSignature.Get();
	pipelineStateStream.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
	pipelineStateStream.PrimitiveTopologyType = inType;
	pipelineStateStream.VS = CD3DX12_SHADER_BYTECODE(shader->GetShader(m_vertexName).ShaderBlob.Get());
	pipelineStateStream.PS = CD3DX12_SHADER_BYTECODE(shader->GetShader(m_pixelName).ShaderBlob.Get());
	pipelineStateStream.DepthStencil = depthStencilDesc;
	pipelineStateStream.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	pipelineStateStream.RTVFormats = rtvFormats;
	pipelineStateStream.Rasterizer = rasterizer;

	D3D12_PIPELINE_STATE_STREAM_DESC pipelineStateStreamDesc = {
		sizeof(PipelineStateStream), &pipelineStateStream
	};

	ThrowIfFailed(device->CreatePipelineState(&pipelineStateStreamDesc, IID_PPV_ARGS(&m_pipelineState)));
}

void PipelineStateScreen::SetupMesh()
{
	// -- Setup Quad Vertices
	std::vector<VertexData> screenVertex;
	screenVertex.push_back(VertexData(glm::vec3(-1.0f, -1.0f, 0.0f), glm::vec3(0.f), glm::vec2(0.0f, 1.0f)));
	screenVertex.push_back(VertexData(glm::vec3(-1.0f, 1.0f, 0.0f), glm::vec3(0.f), glm::vec2(0.0f, 0.0f)));
	screenVertex.push_back(VertexData(glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(0.f), glm::vec2(1.0f, 0.0f)));
	screenVertex.push_back(VertexData(glm::vec3(1.0f, -1.0f, 0.0f), glm::vec3(0.f), glm::vec2(1.0f, 1.0f)));

	// -- Setup Simple Quad Indices
	std::vector<uint16_t> screenIndices;
	screenIndices.push_back(0);
	screenIndices.push_back(1);
	screenIndices.push_back(2);
	screenIndices.push_back(2);
	screenIndices.push_back(3);
	screenIndices.push_back(0);

	// -- Creating the actual mesh
	m_meshScreen = new Mesh(screenVertex, screenIndices);
}

