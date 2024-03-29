#include "precomp.h"
#include "components/terrain/TerrainQuadTree.h"
#include "graphics/Renderer.h"
#include "components/gameobjects/Planet.h"
#include <core/Scene.h>

Planet::Planet(int inDetail, float inSize)
	: m_detail(inDetail), m_planetRadius(inSize)
{

}

Planet::~Planet()
{
}

void Planet::Init()
{
	glm::vec3 origin[6] = {
		glm::vec3(0, m_planetRadius, 0),
		glm::vec3(0, 0, m_planetRadius),
		glm::vec3(0, 0, -m_planetRadius),
		glm::vec3(m_planetRadius, 0, 0),
		glm::vec3(-m_planetRadius, 0, 0),
		glm::vec3(0, -m_planetRadius, 0),
	};
	for (size_t i = 0; i < 6; i++)
	{
		m_quadTree[i] = new TerrainQuadTree(new PlanetTerrain(m_detail, this, origin[i]));

		m_quadTree[i]->Init(m_planetRadius, glm::normalize(origin[i]));
	}
}

void Planet::Update()
{
	for (size_t i = 0; i < 6; i++)
	{
		m_quadTree[i]->Update();
		m_quadTree[i]->m_stopSubdivide = Scene::StopSubdivide();
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

void Planet::FixMeshIndex(uint16_t inIndex)
{
	for (size_t i = 0; i < 6; i++)
	{
		m_quadTree[i]->FixMeshIndex(inIndex);
	}
}
