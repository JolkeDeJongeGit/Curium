#include "precomp.h"
#include "graphics/win32/WinDevice.h"
#include "graphics/ShaderManager.h"
#include "graphics/win32/Pipeline/WinPipelineStateSky.h"
#include <graphics/Mesh.h>
#include "graphics/win32/WinSwapchain.h"
#include "graphics/win32/WinDescriptorHeap.h"
#include "graphics/win32/WinBuffer.h"
#include "graphics/Renderer.h"
#include "graphics/Camera.h"
#include <core/Scene.h>
#include <components/gameobjects/Planet.h>
#include <common/AssetManager.h>

PipelineStateSky::PipelineStateSky(const std::string& inVertexName, const std::string& inPixelName, const D3D12_PRIMITIVE_TOPOLOGY_TYPE inType, const bool inUseDepth)
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
	m_cameraConstant->CreateConstantBuffer(16 * sizeof(float));
}

void PipelineStateSky::Render(ComPtr<ID3D12GraphicsCommandList> commandList)
{
	DescriptorHeap* dsvHeap = WinUtil::GetDescriptorHeap(HeapType::DSV);
	DescriptorHeap* srvHeap = WinUtil::GetDescriptorHeap(HeapType::CBV_SRV_UAV);
	DescriptorHeap* rtvHeap = WinUtil::GetDescriptorHeap(HeapType::RTV);

	ID3D12Resource* renderTarget = WinUtil::GetSwapchain()->GetRenderTextureBuffer().Get();

	// -- Set Pipeline State
	commandList->SetGraphicsRootSignature(m_rootSignature.Get());
	commandList->SetPipelineState(m_pipelineState.Get());

	// -- World position Camera
	Camera* camera = Renderer::GetCamera();

	glm::mat4 view = glm::lookAt(glm::vec3(0.0f), camera->GetTransform().GetForwardVector(), camera->GetTransform().GetUpVector());
	glm::mat4 projection = camera->GetProjection();
	glm::mat4 mvp = projection * view;

	if (auto heightmapTexture = m_meshScreen->m_textureData.find("Sky"); heightmapTexture != m_meshScreen->m_textureData.end())
	{
		auto& texture = heightmapTexture->second;
		auto descriptorIndex = texture.GetDescriptorIndex();
		commandList->SetGraphicsRootDescriptorTable(0, WinUtil::GetDescriptorHeap(HeapType::CBV_SRV_UAV)->GetGpuHandleAt(descriptorIndex));
	}

	struct Bozo
	{
		glm::mat4 mvp;
	};
	Bozo bozo = { mvp };
	// -- Update Camera data
	m_cameraConstant->UpdateBuffer(&bozo);
	m_cameraConstant->SetGraphicsRootConstantBufferView(commandList, 1);
	commandList->SetGraphicsRoot32BitConstants(2, 3, &camera->GetTransform().GetForwardVector(), 0);

	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// -- Draw Screen Quad
	commandList->IASetVertexBuffers(0, 1, &m_meshScreen->GetVertexView());
	commandList->IASetIndexBuffer(&m_meshScreen->GetIndexView());
	commandList->DrawIndexedInstanced(static_cast<UINT>(m_meshScreen->m_indexData.size()), 1, 0, 0, 0);

}

void PipelineStateSky::SetupRootSignature()
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
	CD3DX12_ROOT_PARAMETER1 mvpBuffer;
	ZeroMemory(&mvpBuffer, sizeof(mvpBuffer));
	mvpBuffer.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; // constant buffer
	mvpBuffer.Descriptor = { 0, 0 }; // first register (b0) in first register space
	mvpBuffer.ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX; // only used in _PIXEL shader
	// -- Setup Root Parameters
	CD3DX12_ROOT_PARAMETER1 rootParameter[3];
	rootParameter[0].InitAsDescriptorTable(1, &descRenderRange[0], D3D12_SHADER_VISIBILITY_PIXEL);
	rootParameter[1] = mvpBuffer;
	rootParameter[2].InitAsConstants(3, 1, 0, D3D12_SHADER_VISIBILITY_VERTEX);

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

void PipelineStateSky::SetupPipelineState(D3D12_PRIMITIVE_TOPOLOGY_TYPE inType, bool inUseDepth)
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
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	CD3DX12_RASTERIZER_DESC rasterizer{ CD3DX12_DEFAULT() };
	rasterizer.CullMode = D3D12_CULL_MODE_FRONT;

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

void PipelineStateSky::SetupMesh()
{
	std::vector<VertexData> screenVertex;
	std::vector<uint16_t> screenIndices;

	const float radius = 100000000.f;
	const float sectorCount = 36.f;
	const float stackCount = 18.f;
	// clear memory of prev arrays
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> texCoords;

	float x, y, z, xy;                              // vertex position
	float nx, ny, nz, lengthInv = 1.0f / radius;    // vertex normal
	float s, t;                                     // vertex texCoord

	float sectorStep = 2.f * glm::pi<float>() / sectorCount;
	float stackStep = glm::pi<float>() / stackCount;
	float sectorAngle, stackAngle;

	for (int i = 0; i <= stackCount; ++i)
	{
		stackAngle = glm::pi<float>() / 2 - i * stackStep;        // starting from pi/2 to -pi/2
		xy = radius * cosf(stackAngle);             // r * cos(u)
		z = radius * sinf(stackAngle);              // r * sin(u)

		// add (sectorCount+1) vertices per stack
		// first and last vertices have same position and normal, but different tex coords
		for (int j = 0; j <= sectorCount; ++j)
		{
			sectorAngle = j * sectorStep;           // starting from 0 to 2pi

			// vertex position (x, y, z)
			x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
			y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)
			vertices.emplace_back(x, y, z);

			// normalized vertex normal (nx, ny, nz)
			nx = x * lengthInv;
			ny = y * lengthInv;
			nz = z * lengthInv;
			normals.emplace_back(nx, ny, nz);

			// vertex tex coord (s, t) range between [0, 1]
			s = static_cast<float>(j) / sectorCount;
			t = static_cast<float>(i) / stackCount;
			texCoords.emplace_back(s, t);
		}
	}

	int k1, k2;
	for (int i = 0; i < stackCount; ++i)
	{
		k1 = i * static_cast<int>(sectorCount + 1);     // beginning of current stack
		k2 = k1 + static_cast<int>(sectorCount) + 1;      // beginning of next stack

		for (int j = 0; j < sectorCount; ++j, ++k1, ++k2)
		{
			// 2 triangles per sector excluding first and last stacks
			// k1 => k2 => k1+1
			if (i != 0)
			{
				screenIndices.push_back(k1);
				screenIndices.push_back(k2);
				screenIndices.push_back(k1 + 1);
			}

			// k1+1 => k2 => k2+1
			if (i != (stackCount - 1))
			{
				screenIndices.push_back(k1 + 1);
				screenIndices.push_back(k2);
				screenIndices.push_back(k2 + 1);
			}
		}
	}

	for (size_t i = 0; i < normals.size(); i++)
	{
		screenVertex.push_back(VertexData(vertices[i], normals[i], texCoords[i]));
	}

	// -- Creating the actual mesh
	m_meshScreen = new Mesh(screenVertex, screenIndices);
	m_meshScreen->m_textureData.insert({"Sky", AssetManager::LoadTexture("assets/textures/sky.hdr")});
}

