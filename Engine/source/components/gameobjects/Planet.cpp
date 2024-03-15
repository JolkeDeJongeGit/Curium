#include "precomp.h"
#include "components/terrain/TerrainQuadTree.h"
#include "graphics/Renderer.h"
#include "components/gameobjects/Planet.h"

Planet::Planet(int inDetail, float inSize)
	: m_detail(inDetail), m_size(inSize)
{

}

Planet::~Planet()
{
}

void Planet::Init()
{
	glm::vec3 origin[6] = {
		glm::vec3(0, m_size, 0),
		glm::vec3(0, 0, m_size),
		glm::vec3(0, 0, -m_size),
		glm::vec3(m_size, 0, 0),
		glm::vec3(-m_size, 0, 0),
		glm::vec3(0, -m_size, 0),
	};
	for (size_t i = 0; i < 6; i++)
	{
		m_quadTree[i] = new TerrainQuadTree(new PlanetTerrain(m_detail, this, origin[i]));

		m_quadTree[i]->Init(m_size, glm::normalize(origin[i]));
	}
}

void Planet::Update()
{
	for (size_t i = 0; i < 6; i++)
	{
		m_quadTree[i]->Update();
	}
}

void Planet::Shutdown()
{
}

void Planet::ClearMesh(uint16_t inIndex)
{
	if (m_meshes.size() > inIndex)
	{
		Renderer::AddRemovedMesh(m_meshes[inIndex]);
		m_meshes.erase(m_meshes.begin() + inIndex);
	}
}
