#include "precomp.h"
#include "Components/GameObject.h"

Material::Material(const glm::vec4& inAmbientColor, const glm::vec4& inDiffuseColor, const glm::vec4& inSpecularColor)
{
	for (size_t i = 0; i < 4; i++)
	{
		const glm::length_t j = static_cast<glm::length_t>(i);
		AmbientColor[i] = inAmbientColor[j];
		DiffuseColor[i] = inDiffuseColor[j];
		SpecularColor[i] = inSpecularColor[j];
	}
}

GameObject::~GameObject()
{
	for (Mesh mesh : m_meshes)
	{
		mesh.Shutdown();
	}
}
