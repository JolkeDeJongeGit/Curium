#pragma once

struct VertexData
{
	float Position[3]; // 4 * 3 = 12 bytes
	float Normal[3];   // 4 * 3 = 12 bytes
	float TexCoord[2]; // 4 * 2 = 8  bytes

	VertexData(const glm::vec3& inPosition, const glm::vec3& inNormal, const glm::vec2& inTexCoord);
};					   //         32 bytes

class Texture
{
public:
	Texture() = default;
	Texture(std::string inPath, std::vector<uint8_t> inData, glm::ivec2 inImageSize);
	~Texture() = default;
	inline ID3D12Resource* GetTexture() const { return m_data.Get(); };
	std::string GetPath() const { return m_path; };
	glm::ivec2 GetSize() const;
	[[nodiscard]] uint32_t  GetDescriptorIndex() const;
private:
	uint32_t m_descriptorIndex;
	struct TexData;
	std::string m_path;
	glm::ivec2 m_imageSize;
	ComPtr<ID3D12Resource> m_data;
};

struct BufferData
{
	ComPtr<ID3D12Resource> m_bufferResource;
	D3D12_VERTEX_BUFFER_VIEW m_vertexView{};
	D3D12_INDEX_BUFFER_VIEW m_indexView{};
};

class Mesh
{
public:
	Mesh()
	{
		SetupCube();
	}

	Mesh(const std::vector<VertexData>& _vertexData, const std::vector<uint16_t>& _indexData, const std::unordered_map<std::string, Texture*>&)
		: m_vertexData(_vertexData), m_indexData(_indexData)
	{
		CreateVertexBuffer();
		CreateIndexBuffer();
		CreateTexturesBuffer();
	}

	void CreateVertexBuffer();
	void CreateIndexBuffer();
	void CreateTexturesBuffer();

	void SetupCube();
	void Draw(glm::mat4 inMatrix, const ComPtr<ID3D12GraphicsCommandList>& inCommandList) const;
private:
	std::vector<VertexData> m_vertexData;
	std::vector<uint16_t>   m_indexData;
	std::vector<uint32_t>   m_textureData;

	BufferData m_vertexBuffer;
	BufferData m_indexBuffer;
};