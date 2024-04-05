#include "precomp.h"
#include "components/gameobjects/Terrain.h"
#include <common/AssetManager.h>
#include <graphics/Renderer.h>

Terrain::Terrain(int inDetail)
	: m_detail(inDetail)
{
}

Terrain::~Terrain()
{
}

void Terrain::Init()
{
	m_inverseDetail = 1.f / m_detail;
}

void Terrain::Update()
{
}

void Terrain::Shutdown()
{
}

void Terrain::ClearMesh(uint16_t inIndex)
{
	if (m_meshes.size() > inIndex)
	{
		Renderer::AddRemovedMesh(m_meshes[inIndex]);
		m_meshes.erase(m_meshes.begin() + inIndex);
	}
}

uint16_t Terrain::GenerateTerrain(glm::vec3 inPoint1, glm::vec3 inPoint2, glm::vec3 inPoint3, glm::vec3 inPoint4)
{
	int div = m_detail;
	m_inverseDetail = 1.f / m_detail;
	m_meshes.emplace_back(Mesh());
	auto& mesh = m_meshes[m_meshes.size() - 1];

	for (int row = 0; row < div; row++)
	{
		for (int col = 0; col < div; col++)
		{
			int index = row * (div + 1) + col;
			mesh.m_indexData.emplace_back(index);
			mesh.m_indexData.emplace_back(index + 1);
			mesh.m_indexData.emplace_back(index + (div + 1) + 1);
			mesh.m_indexData.emplace_back(index + (div + 1));
		}
	}

	glm::vec3 v0 = inPoint1;
	glm::vec3 v1 = inPoint2;
	glm::vec3 v2 = inPoint3;
	glm::vec3 v3 = inPoint4;
	glm::vec3 dir03 = (v3 - v0) * m_inverseDetail;
	glm::vec3 dir12 = (v2 - v1) * m_inverseDetail;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> textureCoords;
	// dir2 and dir3
	for (float i = 0; i < div + 1; i++)
	{
		for (float j = 0; j < div + 1; j++)
		{
			glm::vec3 acrossj = ((v1 + i * dir12) - (v0 + i * dir03)) * m_inverseDetail;
			glm::vec3 crntVec = v0 + i * dir03 + j * acrossj;
			// Position
			vertices.emplace_back(glm::vec3(crntVec.x, crntVec.y, crntVec.z));
			// Tex UV
			textureCoords.emplace_back(glm::vec2(float(j) * m_inverseDetail, float(i) * m_inverseDetail));
		}
	}

	for (size_t i = 0; i < vertices.size(); i++)
	{
		mesh.m_vertexData.emplace_back(VertexData(vertices[i], glm::vec3(0, 1, 0), textureCoords[i]));
	}

	mesh.CreateVertexBuffer();
	mesh.CreateIndexBuffer();

	return (uint16_t)m_meshes.size() - 1;
}
