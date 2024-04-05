#include "precomp.h"
#include <common/AssetManager.h>
#include "components/gameobjects/Planet.h"
#include "components/gameobjects/PlanetTerrain.h"

std::vector<uint16_t> indexData;
bool isDone = false; 
PlanetTerrain::PlanetTerrain(int inDetail, Planet* inPlanet, glm::vec3 inOrigin)
	: m_detail(inDetail), m_planet(inPlanet), m_origin(inOrigin)
{
	if (!isDone)
	{
		isDone = true;
		int div = m_detail;

		for (int row = 0; row < div; row++)
		{
			for (int col = 0; col < div; col++)
			{
				int index = row * (div + 1) + col;
				indexData.emplace_back(index);
				indexData.emplace_back(index + 1);
				indexData.emplace_back(index + (div + 1) + 1);
				indexData.emplace_back(index + (div + 1));
			}
		}
	}
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
	const float inverseSize = 1.f / m_planet->m_planetRadius;
	constexpr float inversePi = 1.f / glm::pi<float>();
	constexpr float inverse2Pi = 1.f / (2.0 * glm::pi<float>());

	auto& meshes = m_planet->GetMeshes();
	meshes.emplace_back(Mesh());
	auto& mesh = meshes[meshes.size() - 1];

	mesh.m_indexData = indexData;

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
			glm::vec3 normVec = glm::normalize(glm::vec3(crntVec.x, crntVec.y, crntVec.z));

			vertices.emplace_back(normVec * m_planet->m_planetRadius);

			float u = 0.5f + std::atan2(normVec.z, normVec.x) * inverse2Pi;
			float v = 0.5f - std::asin(normVec.y) * inversePi;

			textureCoords.emplace_back(u, v);

			glm::vec3 normal = (vertices[vertices.size() - 1] - m_planet->GetTransform().GetPosition()) * inverseSize;

			mesh.m_vertexData.emplace_back(VertexData(vertices[vertices.size() - 1], normal, textureCoords[textureCoords.size() - 1]));
		}
	}
	
	mesh.m_textureData.insert(std::pair("heightmap", AssetManager::LoadTexture("assets/textures/planet.jpg")));

	mesh.CreateVertexBuffer();
	mesh.CreateIndexBuffer();

	return (uint16_t)meshes.size() - 1;
}
