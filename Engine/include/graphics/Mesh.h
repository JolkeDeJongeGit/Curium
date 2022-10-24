#pragma once

struct VertexData
{
	float Position[3]; // 4 * 3 = 12 bytes
	float Normal[3];   // 4 * 3 = 12 bytes
	float TexCoord[2]; // 4 * 2 = 8  bytes
};					   //         32 bytes

class Mesh
{
public:
	void SetupCube();
private:
	std::vector<VertexData> m_vertexData;
	std::vector<uint32_t>   m_indexData;
	std::vector<uint32_t>   m_textureData;
};