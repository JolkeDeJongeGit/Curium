#include "precomp.h"
#include <common/AssetManager.h>
#include "components/gameobjects/Planet.h"
#include "components/gameobjects/PlanetTerrain.h"

PlanetTerrain::PlanetTerrain(int inDetail, Planet* inPlanet, glm::vec3 inOrigin)
	: m_detail(inDetail), m_planet(inPlanet), m_origin(inOrigin)
{
}

PlanetTerrain::~PlanetTerrain()
{
}

void PlanetTerrain::Init()
{
	m_inverseDetail = 1.f / m_detail;
}

void PlanetTerrain::ClearMesh(uint16_t inIndex)
{
	m_planet->ClearMesh(inIndex);
}

void PlanetTerrain::FixMeshIndex(uint16_t inIndex)
{
	m_planet->FixMeshIndex(inIndex);
}

uint16_t PlanetTerrain::GenerateTerrain(glm::vec3 inPoint1, glm::vec3 inPoint2, glm::vec3 inPoint3, glm::vec3 inPoint4)
{
	int div = m_detail;
	m_inverseDetail = 1.f / m_detail;

	auto& meshes = m_planet->GetMeshes();
	meshes.emplace_back(Mesh());
	auto& mesh = meshes[meshes.size() - 1];

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
			vertices.emplace_back(glm::normalize(glm::vec3(crntVec.x, crntVec.y, crntVec.z)) * m_planet->m_size);
			//vertices.emplace_back(glm::vec3(crntVec.x, crntVec.y, crntVec.z));
			// Tex UV
			textureCoords.emplace_back(glm::vec2(float(j) * m_inverseDetail, float(i) * m_inverseDetail));
		}
	}

	for (size_t i = 0; i < vertices.size(); i++)
	{
		mesh.m_vertexData.emplace_back(VertexData(vertices[i], glm::vec3(0, 1, 0), textureCoords[i]));
	}

	mesh.m_textureData.insert(std::pair("heightmap", AssetManager::LoadTexture("assets/textures/base.png")));

	mesh.CreateVertexBuffer();
	mesh.CreateIndexBuffer();

	return (uint16_t)meshes.size() - 1;
}
