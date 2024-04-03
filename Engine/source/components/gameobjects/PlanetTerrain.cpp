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
	float inverseSize = 1.f / m_planet->m_planetRadius;

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
		glm::vec3 acrossj = ((v1 + i * dir12) - (v0 + i * dir03)) * m_inverseDetail;
		for (float j = 0; j < div + 1; j++)
		{
			glm::vec3 crntVec = v0 + i * dir03 + j * acrossj;
			// Position

			glm::vec3 normVec = glm::normalize(glm::vec3(crntVec.x, crntVec.y, crntVec.z));

			vertices.emplace_back(normVec * m_planet->m_planetRadius);
			// Tex UV

			float theta = std::atan2(normVec.z, normVec.x);
			float phi = std::acos(normVec.y);

			// Map theta and phi to UV coordinates
			float u = theta / (2.0f * glm::pi<float>());
			float v = phi / glm::pi<float>();

			// Store the normalized UV coordinates
			textureCoords.emplace_back(u, v);
		}
	}
	
	for (size_t i = 0; i < vertices.size(); i++)
	{
		glm::vec3 normal = (vertices[i] - m_planet->GetTransform().GetPosition() ) * inverseSize;

		mesh.m_vertexData.emplace_back(VertexData(vertices[i], normal, textureCoords[i]));
	}

	mesh.m_textureData.insert(std::pair("heightmap", AssetManager::LoadTexture("assets/textures/planet.jpg")));

	mesh.CreateVertexBuffer();
	mesh.CreateIndexBuffer();

	return (uint16_t)meshes.size() - 1;
}
