#pragma once

struct VertexData
{
	float Position[3]; // 4 * 3 = 12 bytes
	float Normal[3];   // 4 * 3 = 12 bytes
	float TexCoord[2]; // 4 * 2 = 8  bytes

	VertexData(const glm::vec3& position, const glm::vec3& normal, const glm::vec2& texCoord);
};					   //         32 bytes

class Texture
{
public:
	Texture() = default;
	Texture(std::string path, std::vector<uint8_t> data, glm::ivec2 imageSize);
	~Texture();
	inline ID3D12Resource* GetTexture() const { return m_data.Get(); };
	std::string GetPath() const { return m_path; };
	glm::ivec2 GetSize() const { return m_imageSize; };

	uint32_t m_descriptorIndex;
private:
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

	Mesh(const std::vector<VertexData>& _vertexData, const std::vector<uint32_t>& _indexData, const std::unordered_map<std::string, Texture*>&)
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
	void Draw(glm::mat4 _matrix, ComPtr<ID3D12GraphicsCommandList>& commandList);
private:
	std::vector<VertexData> m_vertexData;
	std::vector<uint32_t>   m_indexData;
	std::vector<uint32_t>   m_textureData;

	BufferData m_vertexBuffer;
	BufferData m_indexBuffer;
};