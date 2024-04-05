#pragma once
#include "components/GameObject.h"



class Terrain : public GameObject
{
public:
	Terrain() = default;
	Terrain(int inDetail);
	~Terrain() override; 

	void Init() override;
	void Update() override;
	void Shutdown() override;
	void ClearMesh(uint16_t inIndex);
	uint16_t GenerateTerrain(glm::vec3 inPoint1, glm::vec3 inPoint2, glm::vec3 inPoint3, glm::vec3 inPoint4);
	int m_detail;
private:
	float m_inverseDetail;
};