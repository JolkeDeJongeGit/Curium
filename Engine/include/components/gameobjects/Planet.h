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
	float m_planetRadius = 100000.f;
	float m_atmosphereRadius = 130000.f;
	float m_fallOff = 10.0f;
private:
	int m_detail;

	class TerrainQuadTree* m_quadTree[6];
};