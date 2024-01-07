#pragma once
#include "components/GameObject.h"



class Terrain : public GameObject
{
public:
	Terrain() = default;
	Terrain(int inDetail);
	virtual ~Terrain() override; 

	void Init() override;
	void Update() override;
	void Shutdown() override;
private:
	void GenerateTerrain();
private:
	int m_detail;
};