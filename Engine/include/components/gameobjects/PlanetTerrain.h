#pragma once
#include "components/GameObject.h"

class PlanetTerrain
{
public:
	PlanetTerrain() = default;
	PlanetTerrain(int inDetail, class Planet* inPlanet, glm::vec3 inOrigin);
	~PlanetTerrain();

	void Init();
	void ClearMesh(uint16_t inIndex);
	void FixMeshIndex(uint16_t inIndex);

	uint16_t GenerateTerrain(glm::vec3 inPoint1, glm::vec3 inPoint2, glm::vec3 inPoint3, glm::vec3 inPoint4);
	glm::vec3 m_origin;
	int m_detail;
private:
	class Planet* m_planet;
	float m_inverseDetail;
};