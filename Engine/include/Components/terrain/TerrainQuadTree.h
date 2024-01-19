#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <components/gameobjects/Terrain.h>
#include <common/PerformanceManager.h>

static constexpr uint16_t MaxDepth = 16;

static constexpr uint16_t NORTH = 0;
static constexpr uint16_t EAST = 1;
static constexpr uint16_t WEST = 2;
static constexpr uint16_t SOUTH = 3;

struct TerrainNode
{
	TerrainNode() = default;
	~TerrainNode() = default;

	TerrainNode(glm::dvec2 inPoint, float inSize, int16_t inDepth, TerrainNode* inParentNode)
		: m_point(inPoint), m_size(inSize), m_depth(inDepth), m_parentNode(inParentNode)
	{
	}
	glm::dvec2 m_point = glm::dvec2(0);
	float m_size = -1;
	int16_t m_depth = -1;
	TerrainNode* m_parentNode = nullptr;
	TerrainNode* m_terrainNodes[4];
	int16_t m_meshIndex = -1;
};

class TerrainQuadTree
{
public:
	TerrainQuadTree() = default;
	TerrainQuadTree(Terrain* inTerrain)
		: m_terrain(inTerrain)
	{
	}

	inline ~TerrainQuadTree();
	inline void Init();
	inline void Update(); 
	inline void Subdivide(TerrainNode* inNode);
	inline void UnSubdivide(TerrainNode* inNode);

	bool m_stopSubdivide = false;
private:
	Terrain* m_terrain;
	TerrainNode* m_rootNode;
	std::vector<TerrainNode*> m_leafNodes;
};

inline TerrainQuadTree::~TerrainQuadTree()
{
	m_terrain = nullptr;
}

inline void TerrainQuadTree::Init()
{
	m_rootNode = new TerrainNode();
	m_rootNode->m_depth = 0;
	m_rootNode->m_size = 100000;

	glm::vec3 v0 = glm::vec3(-m_rootNode->m_size, 0, -m_rootNode->m_size);
	glm::vec3 v1 = glm::vec3(-m_rootNode->m_size, 0, m_rootNode->m_size);
	glm::vec3 v2 = glm::vec3(m_rootNode->m_size,0, m_rootNode->m_size);
	glm::vec3 v3 = glm::vec3(m_rootNode->m_size, 0, -m_rootNode->m_size);

	m_rootNode->m_point = glm::dvec2(0,0);
	m_terrain->m_detail = 8;
	m_rootNode->m_meshIndex = m_terrain->GenerateTerrain(v0, v1, v2, v3);
	m_leafNodes.push_back(m_rootNode);
}


inline void TerrainQuadTree::Update()
{
	PROFILE_FUNCTION();
	// Distance check
	auto drawlist = ImGui::GetWindowDrawList();
	if (!m_stopSubdivide)
	{
		for (size_t i = 0; i < m_leafNodes.size(); i++)
		{
			auto leafNode = m_leafNodes[i];
			auto worldPos = WorldToScreen(glm::vec3(leafNode->m_point.x, m_terrain->GetTransform().GetPosition().y, leafNode->m_point.y), Renderer::GetCamera()->GetView(), Renderer::GetCamera()->GetProjection());
			//std::string bozo = "Half size: " + std::to_string(leafNode->m_size);
			std::string bozo = "MeshID: " + std::to_string(leafNode->m_meshIndex);
			drawlist->AddText(ImVec2(worldPos.x, worldPos.y), IM_COL32_WHITE, bozo.c_str());

			if (leafNode->m_depth < MaxDepth && glm::distance(Renderer::GetCamera()->GetTransform().GetPosition(), glm::vec3(leafNode->m_point.x, m_terrain->GetTransform().GetPosition().y, leafNode->m_point.y)) <= leafNode->m_size * 1.5f)
			{
				Subdivide(leafNode);
				return;
			}
			else if(leafNode->m_parentNode)
			{
				if (glm::distance(Renderer::GetCamera()->GetTransform().GetPosition(), glm::vec3(leafNode->m_parentNode->m_point.x, m_terrain->GetTransform().GetPosition().y, leafNode->m_parentNode->m_point.y)) > leafNode->m_parentNode->m_size * 1.5)
				{
					UnSubdivide(leafNode->m_parentNode);
					return;
				}
			}
		}
	}
}

inline void TerrainQuadTree::Subdivide(TerrainNode* inNode)
{
	PROFILE_FUNCTION()
	TerrainNode* node = inNode;

	m_terrain->ClearMesh(node->m_meshIndex);

	m_leafNodes.erase(std::remove(m_leafNodes.begin(), m_leafNodes.end(), inNode), m_leafNodes.end());

	for (TerrainNode* terrain : m_leafNodes)
	{
		if (terrain->m_meshIndex > node->m_meshIndex)
			terrain->m_meshIndex--;
	}

	uint16_t depth = node->m_depth;
	float adjustedSize = node->m_size * .5f;

	TerrainNode* topLeft = new TerrainNode(glm::dvec2(node->m_point.x - adjustedSize, node->m_point.y + adjustedSize), adjustedSize, depth + 1, node);
	glm::vec3 v0 = glm::vec3(node->m_point.x - node->m_size, 0, node->m_point.y);
	glm::vec3 v1 = glm::vec3(node->m_point.x - node->m_size, 0, node->m_point.y + node->m_size);
	glm::vec3 v2 = glm::vec3(node->m_point.x, 0, node->m_point.y + node->m_size);
	glm::vec3 v3 = glm::vec3(node->m_point.x, 0, node->m_point.y);
	topLeft->m_meshIndex = m_terrain->GenerateTerrain(v0, v1, v2, v3);

	TerrainNode* topRight = new TerrainNode(glm::dvec2(node->m_point.x + adjustedSize, node->m_point.y + adjustedSize), adjustedSize, depth + 1, node);
	v0 = glm::vec3(node->m_point.x, 0, node->m_point.y);
	v1 = glm::vec3(node->m_point.x, 0, node->m_point.y + node->m_size);
	v2 = glm::vec3(node->m_point.x + node->m_size, 0, node->m_point.y + node->m_size);
	v3 = glm::vec3(node->m_point.x + node->m_size, 0, node->m_point.y);
	topRight->m_meshIndex = m_terrain->GenerateTerrain(v0, v1, v2, v3);

	TerrainNode* bottomRight = new TerrainNode(glm::dvec2(node->m_point.x - adjustedSize, node->m_point.y - adjustedSize), adjustedSize, depth + 1, node);
	v0 = glm::vec3(node->m_point.x, 0, node->m_point.y - node->m_size);
	v1 = glm::vec3(node->m_point.x, 0, node->m_point.y);
	v2 = glm::vec3(node->m_point.x + node->m_size, 0, node->m_point.y);
	v3 = glm::vec3(node->m_point.x + node->m_size, 0, node->m_point.y - node->m_size);
	bottomRight->m_meshIndex = m_terrain->GenerateTerrain(v0, v1, v2, v3);

	TerrainNode* bottomLeft = new TerrainNode(glm::dvec2(node->m_point.x + adjustedSize, node->m_point.y - adjustedSize), adjustedSize, depth + 1, node);
	v0 = glm::vec3(node->m_point.x - node->m_size, 0, node->m_point.y - node->m_size);
	v1 = glm::vec3(node->m_point.x - node->m_size, 0, node->m_point.y);
	v2 = glm::vec3(node->m_point.x, 0, node->m_point.y);
	v3 = glm::vec3(node->m_point.x, 0, node->m_point.y - node->m_size);
	bottomLeft->m_meshIndex = m_terrain->GenerateTerrain(v0, v1, v2, v3);

	node->m_terrainNodes[0] = topLeft;
	node->m_terrainNodes[1] = topRight;
	node->m_terrainNodes[2] = bottomRight;
	node->m_terrainNodes[3] = bottomLeft;

	m_leafNodes.push_back(topLeft);
	m_leafNodes.push_back(topRight);
	m_leafNodes.push_back(bottomRight);
	m_leafNodes.push_back(bottomLeft);
	node->m_meshIndex = -1;
}

inline void TerrainQuadTree::UnSubdivide(TerrainNode* inNode)
{
	for (size_t i = 0; i < 4; i++)
	{
		auto& deleteNode = inNode->m_terrainNodes[i];
		m_terrain->ClearMesh(deleteNode->m_meshIndex);

		m_leafNodes.erase(std::remove(m_leafNodes.begin(), m_leafNodes.end(), deleteNode), m_leafNodes.end());

		for (TerrainNode* terrain : m_leafNodes)
		{
			if (terrain->m_meshIndex > deleteNode->m_meshIndex)
				terrain->m_meshIndex--;
		}

		if (deleteNode)
		{
			delete deleteNode;
			deleteNode = nullptr;
		}
	}

	glm::vec3 v0 = glm::vec3(inNode->m_point.x - inNode->m_size, 0.0f, inNode->m_point.y - inNode->m_size);
	glm::vec3 v1 = glm::vec3(inNode->m_point.x - inNode->m_size, 0.0f, inNode->m_point.y + inNode->m_size);
	glm::vec3 v2 = glm::vec3(inNode->m_point.x + inNode->m_size, 0.0f, inNode->m_point.y + inNode->m_size);
	glm::vec3 v3 = glm::vec3(inNode->m_point.x + inNode->m_size, 0.0f, inNode->m_point.y - inNode->m_size);
	inNode->m_meshIndex = m_terrain->GenerateTerrain(v0, v1, v2, v3);

	m_leafNodes.push_back(inNode);
}
