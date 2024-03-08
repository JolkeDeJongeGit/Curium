#pragma once

struct VertexData
{
	float Position[3]; // 4 * 3 = 12 bytes
	float Normal[3];   // 4 * 3 = 12 bytes
	float TexCoord[2]; // 4 * 2 = 8  bytes
	VertexData() = default;
	VertexData(const glm::vec3& inPosition, const glm::vec3& inNormal, const glm::vec2& inTexCoord);
};					   //         32 bytes

class Texture
{
public:
	Texture() = default;

	Texture(std::string inPath, std::vector<uint8_t> inData, glm::ivec2 inImageSize);
	~Texture() = default;

	void SetState(D3D12_RESOURCE_STATES inSetState);

	inline ID3D12Resource* GetTexture() const { return m_data.Get(); };
	glm::ivec2 GetSize() const;
	[[nodiscard]] uint32_t  GetDescriptorIndex() const;
private:
	glm::ivec2 m_imageSize = glm::ivec2(0,0);
	ComPtr<ID3D12Resource> m_data;
	D3D12_RESOURCE_STATES m_currentState = D3D12_RESOURCE_STATE_COMMON;
	uint32_t m_descriptorIndex = -1;
};

class Mesh
{
public:
	Mesh() = default; 
	Mesh(const std::vector<VertexData>& inVertexData, const std::vector<uint16_t>& inIndexData)
		: m_vertexData(inVertexData), m_indexData(inIndexData)
	{
		CreateVertexBuffer();
		CreateIndexBuffer();
		CreateTexturesBuffer();
	}

	void Shutdown();

	void CreateVertexBuffer();
	void CreateIndexBuffer();
	void CreateTexturesBuffer();
	
	void SetupCube();
	void SetupSphere();
	void Draw(const ComPtr<ID3D12GraphicsCommandList>& inCommandList) const;

	std::vector<VertexData> m_vertexData;
	std::vector<uint16_t>   m_indexData;
	std::unordered_map<std::string, Texture>  m_textureData;
	ComPtr<ID3D12Resource> m_vertexBuffer;
	ComPtr<ID3D12Resource> m_indexBuffer;

	bool m_cull = false;
private:
	D3D12_VERTEX_BUFFER_VIEW m_vertexView{};
	D3D12_INDEX_BUFFER_VIEW m_indexView{};
};