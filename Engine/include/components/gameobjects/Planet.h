#pragma once
#include "components/GameObject.h"

class Planet : public GameObject
{
public:
	Planet() = default;
	Planet(int inDetail, float inSize);
	~Planet() override;

	void Init() override;
	void Update() override;
	void Shutdown() override;
	void ClearMesh(uint16_t inIndex);
	void FixMeshIndex(uint16_t inIndex);
	float m_size;
private:
	int m_detail;

	class TerrainQuadTree* m_quadTree[6];
};