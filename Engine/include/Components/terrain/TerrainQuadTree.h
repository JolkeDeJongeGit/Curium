#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <components/gameobjects/Terrain.h>
#include <common/PerformanceManager.h>

static constexpr uint16_t MaxDepth = 4;

static constexpr uint16_t NORTH = 0;
static constexpr uint16_t EAST = 1;
static constexpr uint16_t WEST = 2;
static constexpr uint16_t SOUTH = 3;

struct TerrainNode
{
    TerrainNode() = default;
    ~TerrainNode() = default;

    TerrainNode(const TerrainNode&) = delete;
    TerrainNode(TerrainNode&&) = delete;
    TerrainNode& operator=(const TerrainNode&) = delete;
    TerrainNode& operator=(TerrainNode&&) = delete;

    TerrainNode(glm::dvec2 inPoint, float inSize, int16_t inDepth, TerrainNode* inParentNode)
        : m_point(inPoint), m_size(inSize), m_depth(inDepth), m_parentNode(inParentNode)
    {
    }

    void Reset(Terrain* inTerrain, std::vector<TerrainNode*>& list)
    {
        for (size_t i = 0; i < 4; i++)
        {
            auto node = m_terrainNodes[i];
            if (node)
            {
                node->Reset(inTerrain, list);

                delete node;
                m_terrainNodes[i] = nullptr;
            }
        }

        if (m_meshIndex != -1)
        {
            inTerrain->ClearMesh(m_meshIndex);

            list.erase(std::remove(list.begin(), list.end(), this), list.end());

            for (TerrainNode* terrain : list)
            {
                if (terrain->m_meshIndex > m_meshIndex)
                    terrain->m_meshIndex--;
            }

            m_meshIndex = -1;
        }
    }

    glm::dvec2 m_point = glm::dvec2(0);
    float m_size = -1;
    int16_t m_depth = -1;
    TerrainNode* m_parentNode = nullptr;
    TerrainNode* m_terrainNodes[4] = { nullptr, nullptr, nullptr, nullptr };
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
    glm::vec3 v2 = glm::vec3(m_rootNode->m_size, 0, m_rootNode->m_size);
    glm::vec3 v3 = glm::vec3(m_rootNode->m_size, 0, -m_rootNode->m_size);

    m_rootNode->m_point = glm::dvec2(0, 0);
    m_terrain->m_detail = 4;
    m_rootNode->m_meshIndex = m_terrain->GenerateTerrain(v0, v1, v2, v3);
    m_leafNodes.push_back(m_rootNode);
}


bool IsNodeInsideFrustum(const glm::vec3& nodePos, const std::vector<Plane>& frustumPlanes) {
    for (const auto& plane : frustumPlanes) {
        // Compute the signed distance from the node's position to the plane
        float distance = glm::dot(plane.normal, nodePos) + plane.distance;

        // If the node is behind any of the frustum planes, it's outside the frustum
        if (distance < 0.0f)
            return false;
    }
    return true;
}

inline void TerrainQuadTree::Update()
{
    PROFILE_FUNCTION();
    // Distance check
    auto drawlist = ImGui::GetWindowDrawList();

    std::vector<Plane> frustumPlanes = Renderer::GetCamera()->GetFrustum().GetPlanes(); // Function to get frustum planes
    const auto sseThreshold = 0.3f;

    const float distanceThreshold = 10.0f; // Adjust this threshold as needed
    auto& cameraPos = Renderer::GetCamera()->GetTransform().GetPosition();
    auto& terrainPos = m_terrain->GetTransform().GetPosition();

    Renderer::GetCamera()->m_updateFrustum = !m_stopSubdivide;

    for (size_t i = 0; i < m_leafNodes.size(); i++) 
    {
        auto leafNode = m_leafNodes[i];

        const auto nodePos1 = glm::vec3{ leafNode->m_point.x, 0.0f, leafNode->m_point.y } + terrainPos;

        // Compute the distance from the camera to the node's center

        if (IsNodeInsideFrustum(nodePos1, frustumPlanes))
        {
            auto worldPos = WorldToScreen(glm::vec3(leafNode->m_point.x, m_terrain->GetTransform().GetPosition().y, leafNode->m_point.y), Renderer::GetCamera()->GetView(), Renderer::GetCamera()->GetProjection());
            std::string bozo1 = "In Frustrum";
            auto v0 = leafNode->m_point;
            auto world = WorldToScreen(glm::vec3(v0[0] - 100, m_terrain->GetTransform().GetPosition().y + 500 * i, v0[1]), Renderer::GetCamera()->GetView(), Renderer::GetCamera()->GetProjection());
            drawlist->AddText(ImVec2(world.x, world.y), IM_COL32(170, 0, 255, 255), bozo1.c_str());
        }

        if (!IsNodeInsideFrustum(nodePos1, frustumPlanes))
            continue;

        const auto d1 = glm::length(cameraPos - nodePos1);

        // Compute the screen space error of the node
        const auto sse = leafNode->m_size / d1;
        if (!m_stopSubdivide)
        {

            if (leafNode->m_depth < MaxDepth && sse > sseThreshold) {
                Subdivide(leafNode);
            }
            else if (leafNode->m_parentNode) {
                // Compute the world-space position of the parent node
                const auto nodePos2 = glm::vec3{ leafNode->m_parentNode->m_point.x, 0.0f, leafNode->m_parentNode->m_point.y } + terrainPos;
                const auto d2 = glm::length(cameraPos - nodePos2);

                // Compute the screen space error of the parent node
                const auto sseParent = leafNode->m_parentNode->m_size / d2;
                if (sseParent < sseThreshold) {
                    UnSubdivide(leafNode->m_parentNode);
                }
            }
        }
    }
}

inline void TerrainQuadTree::Subdivide(TerrainNode* node)
{
    PROFILE_FUNCTION()

    m_terrain->ClearMesh(node->m_meshIndex);

    m_leafNodes.erase(std::remove(m_leafNodes.begin(), m_leafNodes.end(), node), m_leafNodes.end());

    for (TerrainNode* terrain : m_leafNodes)
    {
        if (terrain->m_meshIndex > node->m_meshIndex)
            terrain->m_meshIndex--;
    }

    int16_t depth = static_cast<int16_t>( node->m_depth + 1 );
    float adjustedSize = node->m_size * 0.5f;

    TerrainNode* topLeft = new TerrainNode(glm::dvec2(node->m_point.x - adjustedSize, node->m_point.y + adjustedSize), adjustedSize, depth, node);
    glm::vec3 v0 = glm::vec3(node->m_point.x - node->m_size, 0, node->m_point.y);
    glm::vec3 v1 = glm::vec3(node->m_point.x - node->m_size, 0, node->m_point.y + node->m_size);
    glm::vec3 v2 = glm::vec3(node->m_point.x, 0, node->m_point.y + node->m_size);
    glm::vec3 v3 = glm::vec3(node->m_point.x, 0, node->m_point.y);
    topLeft->m_meshIndex = m_terrain->GenerateTerrain(v0, v1, v2, v3);

    TerrainNode* topRight = new TerrainNode(glm::dvec2(node->m_point.x + adjustedSize, node->m_point.y + adjustedSize), adjustedSize, depth, node);
    v0 = glm::vec3(node->m_point.x, 0, node->m_point.y);
    v1 = glm::vec3(node->m_point.x, 0, node->m_point.y + node->m_size);
    v2 = glm::vec3(node->m_point.x + node->m_size, 0, node->m_point.y + node->m_size);
    v3 = glm::vec3(node->m_point.x + node->m_size, 0, node->m_point.y);
    topRight->m_meshIndex = m_terrain->GenerateTerrain(v0, v1, v2, v3);

    TerrainNode* bottomRight = new TerrainNode(glm::dvec2(node->m_point.x + adjustedSize, node->m_point.y - adjustedSize), adjustedSize, depth, node);
    v0 = glm::vec3(node->m_point.x, 0, node->m_point.y - node->m_size);
    v1 = glm::vec3(node->m_point.x, 0, node->m_point.y);
    v2 = glm::vec3(node->m_point.x + node->m_size, 0, node->m_point.y);
    v3 = glm::vec3(node->m_point.x + node->m_size, 0, node->m_point.y - node->m_size);
    bottomRight->m_meshIndex = m_terrain->GenerateTerrain(v0, v1, v2, v3);

    TerrainNode* bottomLeft = new TerrainNode(glm::dvec2(node->m_point.x - adjustedSize, node->m_point.y - adjustedSize), adjustedSize, depth, node);
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

        if (deleteNode)
        {
            deleteNode->Reset(m_terrain, m_leafNodes);
        }
    }

    glm::vec3 v0 = glm::vec3(inNode->m_point.x - inNode->m_size, 0.0f, inNode->m_point.y - inNode->m_size);
    glm::vec3 v1 = glm::vec3(inNode->m_point.x - inNode->m_size, 0.0f, inNode->m_point.y + inNode->m_size);
    glm::vec3 v2 = glm::vec3(inNode->m_point.x + inNode->m_size, 0.0f, inNode->m_point.y + inNode->m_size);
    glm::vec3 v3 = glm::vec3(inNode->m_point.x + inNode->m_size, 0.0f, inNode->m_point.y - inNode->m_size);
    inNode->m_meshIndex = m_terrain->GenerateTerrain(v0, v1, v2, v3);

    m_leafNodes.push_back(inNode);
}
