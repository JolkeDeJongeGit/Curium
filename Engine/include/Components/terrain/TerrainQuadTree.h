#pragma once

enum class TerrainNodeDirection
{
	North = 0,
	East = 1,
	West = 2,
	South = 3
};

class TerrainNode
{
public:

	inline void Subdivide();
private:
	TerrainNode* m_childeren[4]; 
};