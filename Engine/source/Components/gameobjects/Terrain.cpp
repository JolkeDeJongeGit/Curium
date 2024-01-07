#include "precomp.h"
#include "components/gameobjects/Terrain.h"
#include <common/AssetManager.h>

Terrain::Terrain(int inDetail)
	: m_detail(inDetail)
{
	GenerateTerrain();
	m_meshes[0].m_textureData.insert(std::pair("heightmap", AssetManager::LoadTexture("assets/textures/heightmap.png")));
}

Terrain::~Terrain()
{
}

void Terrain::Init()
{
}

void Terrain::Update()
{
}

void Terrain::Shutdown()
{
}

void Terrain::GenerateTerrain()
{
	int div = m_detail;

	m_meshes.push_back(Mesh());
	auto& mesh = m_meshes[0];

	for (int row = 0; row < div; row++)
	{
		for (int col = 0; col < div; col++)
		{
			int index = row * (div + 1) + col;
			mesh.m_indexData.push_back(index);
			mesh.m_indexData.push_back(index + 1);
			mesh.m_indexData.push_back(index + (div + 1) + 1);
			mesh.m_indexData.push_back(index + (div + 1));
		}
	}

	glm::vec3 v0 = glm::vec3(-0.5f, 0.0f, -0.5f);
	glm::vec3 v1 = glm::vec3(-0.5f, 0.0f, 0.5f);
	glm::vec3 v2 = glm::vec3(0.5f, 0.0f, 0.5f);
	glm::vec3 v3 = glm::vec3(0.5f, 0.0f, -0.5f);

	glm::vec3 dir03 = (v3 - v0) / float(div);
	glm::vec3 dir12 = (v2 - v1) / float(div);
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> textureCoords;
	// dir2 and dir3
	for (float i = 0; i < div + 1; i++)
	{
		for (float j = 0; j < div + 1; j++)
		{
			glm::vec3 acrossj = ((v1 + i * dir12) - (v0 + i * dir03)) / float(div);
			glm::vec3 crntVec = v0 + i * dir03 + j * acrossj;
			// Position
			vertices.push_back(glm::vec3(crntVec.x, crntVec.y, crntVec.z));
			// Tex UV
			textureCoords.push_back(glm::vec2(float(j) / div, float(i) / div));
		}
	}

	for (size_t i = 0; i < vertices.size(); i++)
	{
		mesh.m_vertexData.push_back(VertexData(vertices[i], glm::vec3(0, 1, 0), textureCoords[i]));
	}
}
