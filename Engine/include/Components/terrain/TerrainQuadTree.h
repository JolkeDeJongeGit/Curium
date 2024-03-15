#pragma once
#include <glm/glm.hpp>
#include <vector>
#include "components/gameobjects/PlanetTerrain.h"
#include "common/PerformanceManager.h"
#include "graphics/Camera.h"
#include "graphics/Renderer.h"

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

    TerrainNode(glm::vec3 inPoint, float inSize, int16_t inDepth, TerrainNode* inParentNode)
        : m_point(inPoint), m_size(inSize), m_depth(inDepth), m_parentNode(inParentNode)
    {
    }

    void Reset(PlanetTerrain* inTerrain, std::vector<TerrainNode*>& list)
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


            inTerrain->FixMeshIndex(m_meshIndex);

            m_meshIndex = -1;
        }
    }

    glm::vec3 m_point = glm::vec3(0);
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
    TerrainQuadTree(PlanetTerrain* inTerrain)
        : m_terrain(inTerrain)
    {
    }

    inline ~TerrainQuadTree();
    inline void Init(float inSize, glm::vec3 inNormal);
    inline void Update();
    inline void Subdivide(TerrainNode* inNode);
    inline void UnSubdivide(TerrainNode* inNode);
    inline void FixMeshIndex(int16_t inIndex);
    bool m_stopSubdivide = false;
private:
    PlanetTerrain* m_terrain;
    TerrainNode* m_rootNode;
    std::vector<TerrainNode*> m_leafNodes;
    int16_t m_normal[3];
};

inline TerrainQuadTree::~TerrainQuadTree()
{
    m_terrain = nullptr;
}

inline void TerrainQuadTree::Init(float inSize, glm::vec3 inNormal)
{
    for (size_t i = 0; i < 3; i++)
    {
        m_normal[i] = static_cast<int16_t>(inNormal[i]) + 2;
    }

    m_rootNode = new TerrainNode();
    m_rootNode->m_depth = 0;
    m_rootNode->m_size = inSize;

    // Calculate right and forward vectors based on the normal
    glm::vec3 right;
    if (inNormal == glm::vec3(0, 1, 0)) {
        right = glm::vec3(1, 0, 0);
    }
    else if (inNormal == glm::vec3(0, -1, 0))
    {
        right = glm::vec3(-1, 0, 0);
    }
    else {
        // Calculate right vector based on the normal
        glm::vec3 up(0, 1, 0);
        right = glm::cross(inNormal, up);
    }
    glm::vec3 forward = glm::cross(right, inNormal);

    // Define vertices of the terrain plane using the calculated vectors
    glm::vec3 v0 = -inSize * right - inSize * forward + m_terrain->m_origin;
    glm::vec3 v1 = -inSize * right + inSize * forward + m_terrain->m_origin;
    glm::vec3 v2 = inSize * right + inSize * forward + m_terrain->m_origin;
    glm::vec3 v3 = inSize * right - inSize * forward + m_terrain->m_origin;

    // Set the point of the root node (assuming it's at the center for simplicity)
    m_rootNode->m_point = m_terrain->m_origin;

    // Generate terrain using the calculated vertices
    m_rootNode->m_meshIndex = m_terrain->GenerateTerrain(v0 , v1 , v2, v3);

    // Add the root node to the leaf nodes list
    m_leafNodes.push_back(m_rootNode);
}

//bool IsNodeInsideFrustum(const glm::vec3& nodePos, const std::vector<Plane>& frustumPlanes) {
//    for (const auto& plane : frustumPlanes) {
//        // Compute the signed distance from the node's position to the plane
//        float distance = glm::dot(plane.normal, nodePos) + plane.distance;
//
//        // If the node is behind any of the frustum planes, it's outside the frustum
//        if (distance < 0.0f)
//            return false;
//    }
//    return true;
//}

inline void TerrainQuadTree::Update()
{
    PROFILE_FUNCTION();
    std::vector<Plane> frustumPlanes = Renderer::GetCamera()->GetFrustum().GetPlanes(); // Function to get frustum planes
    const auto sseThreshold = 0.4f;

    auto& cameraPos = Renderer::GetCamera()->GetTransform().GetPosition();
    auto& terrainPos = m_terrain->m_origin;

    Renderer::GetCamera()->m_updateFrustum = !m_stopSubdivide;

    //for (size_t i = 0; i < m_leafNodes.size(); i++) 
    //{
    //    auto leafNode = m_leafNodes[i];

    //    const auto nodePos1 = leafNode->m_point + terrainPos;

    //    //const auto topLeft = glm::vec3{ leafNode->m_point.x - leafNode->m_size, 0.0f, leafNode->m_point.y + leafNode->m_size } + terrainPos;
    //    //const auto topRight = glm::vec3{ leafNode->m_point.x + leafNode->m_size, 0.0f, leafNode->m_point.y + leafNode->m_size } + terrainPos;
    //    //const auto bottomLeft = glm::vec3{ leafNode->m_point.x - leafNode->m_size, 0.0f, leafNode->m_point.y - leafNode->m_size } + terrainPos;
    //    //const auto bottomRight = glm::vec3{ leafNode->m_point.x + leafNode->m_size, 0.0f, leafNode->m_point.y - leafNode->m_size } + terrainPos;

    //    //if (!IsNodeInsideFrustum(topLeft, frustumPlanes)
    //    //    && !IsNodeInsideFrustum(topRight, frustumPlanes)
    //    //    && !IsNodeInsideFrustum(bottomLeft, frustumPlanes)
    //    //    && !IsNodeInsideFrustum(bottomRight, frustumPlanes))
    //    //{
    //    //    //m_terrain->GetMeshes()[leafNode->m_meshIndex].m_cull = true;
    //    //    continue;
    //    //}

    //    //m_terrain->GetMeshes()[leafNode->m_meshIndex].m_cull = false;

    //    const auto d1 = glm::length(cameraPos - nodePos1);

    //    // Compute the screen space error of the node
    //    const auto sse = leafNode->m_size / d1;
    //    if (!m_stopSubdivide)
    //    {
    //        if (leafNode->m_depth < MaxDepth && sse > sseThreshold) {
    //            Subdivide(leafNode);
    //        }
    //        else if (leafNode->m_parentNode) {
    //            // Compute the world-space position of the parent node
    //            const auto nodePos2 = leafNode->m_parentNode->m_point + terrainPos;
    //            const auto d2 = glm::length(cameraPos - nodePos2);

    //            // Compute the screen space error of the parent node
    //            const auto sseParent = leafNode->m_parentNode->m_size / d2;
    //            if (sseParent > sseThreshold) {
    //                UnSubdivide(leafNode->m_parentNode);
    //            }
    //        }
    //    }
    //}
}

inline void TerrainQuadTree::Subdivide(TerrainNode* node)
{
    PROFILE_FUNCTION()

    m_terrain->ClearMesh(node->m_meshIndex);

    m_leafNodes.erase(std::remove(m_leafNodes.begin(), m_leafNodes.end(), node), m_leafNodes.end());

    m_terrain->FixMeshIndex(node->m_meshIndex);

    int16_t depth = static_cast<int16_t>(node->m_depth + 1);
    float adjustedSize = node->m_size * 0.5f;

    // Define a normal vector for the plane (modify this based on the direction)
    // - 2 is so that if my normal = 1 it will -1 and 3 will make 1. This is so that I can keep my terrain node cache friendly
    glm::vec3 normal(static_cast<float>(m_normal[0]) - 2, static_cast<float>(m_normal[1]) - 2, static_cast<float>(m_normal[2]) - 2);

    glm::vec3 right;
    if (normal == glm::vec3(0, 1, 0)) {
        right = glm::vec3(1, 0, 0);
    }
    else if (normal == glm::vec3(0, -1, 0))
    {
        right = glm::vec3(-1, 0, 0);
    }
    else {
        // Calculate right vector based on the normal
        glm::vec3 up(0, 1, 0);
        right = glm::cross(normal, up);
    }
    glm::vec3 forward = glm::cross(right, normal);

    glm::vec3 point(node->m_point);

    // Array to store the terrain nodes
    TerrainNode* terrainNodes[4];

    TerrainNode* topLeft = new TerrainNode(point - adjustedSize * right + adjustedSize * forward, adjustedSize, depth, node);
    glm::vec3 v0 = point - node->m_size * right;
    glm::vec3 v1 = point - node->m_size * right + node->m_size * forward;
    glm::vec3 v2 = point + node->m_size * forward;
    glm::vec3 v3 = point;
    topLeft->m_meshIndex = m_terrain->GenerateTerrain(v0, v1, v2, v3);

    TerrainNode* topRight = new TerrainNode(point + adjustedSize * right + adjustedSize * forward, adjustedSize, depth, node);
    v0 = point;
    v1 = point + node->m_size * forward;
    v2 = point + node->m_size * right + node->m_size * forward;
    v3 = point + node->m_size * right;
    topRight->m_meshIndex = m_terrain->GenerateTerrain(v0, v1, v2, v3);

    TerrainNode* bottomRight = new TerrainNode(point + adjustedSize * right - adjustedSize * forward, adjustedSize, depth, node);
    v0 = point - node->m_size * forward;
    v1 = point;
    v2 = point + node->m_size * right;
    v3 = point + node->m_size * right - node->m_size * forward;
    bottomRight->m_meshIndex = m_terrain->GenerateTerrain(v0, v1, v2, v3);

    TerrainNode* bottomLeft = new TerrainNode(point - adjustedSize * right - adjustedSize * forward, adjustedSize, depth, node);
    v0 = point - node->m_size * right - node->m_size * forward;
    v1 = point - node->m_size * right;
    v2 = point;
    v3 = point - node->m_size * forward;
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

    // Define a normal vector for the plane (modify this based on the direction)
    // - 2 is so that if my normal = 1 it will -1 and 3 will make 1. This is so that I can keep my terrain node cache friendly
    glm::vec3 normal(static_cast<float>(m_normal[0]) - 2, static_cast<float>(m_normal[1]) - 2, static_cast<float>(m_normal[2]) - 2);

    glm::vec3 right;
    if (normal == glm::vec3(0, 1, 0)) {
        right = glm::vec3(1, 0, 0);
    }
    else if (normal == glm::vec3(0, -1, 0))
    {
        right = glm::vec3(-1, 0, 0);
    }
    else {
        // Calculate right vector based on the normal
        glm::vec3 up(0, 1, 0);
        right = glm::cross(normal, up);
    }
    glm::vec3 forward = glm::cross(right, normal);

    glm::vec3 v0 = inNode->m_point - inNode->m_size * right - inNode->m_size * forward;
    glm::vec3 v1 = inNode->m_point - inNode->m_size * right + inNode->m_size * forward;
    glm::vec3 v2 = inNode->m_point + inNode->m_size * right + inNode->m_size * forward;
    glm::vec3 v3 = inNode->m_point + inNode->m_size * right - inNode->m_size * forward;

    inNode->m_meshIndex = m_terrain->GenerateTerrain(v0, v1, v2, v3);

    m_leafNodes.push_back(inNode);
}

inline void TerrainQuadTree::FixMeshIndex(int16_t inIndex)
{
    for (TerrainNode* terrain : m_leafNodes)
    {
        if (terrain->m_meshIndex > inIndex)
            terrain->m_meshIndex--;
    }
}
