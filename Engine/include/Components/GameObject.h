#pragma once
#include "Transform.h"
#include "graphics/Mesh.h"

struct Material
{
  Material() = default;
  Material(const glm::vec4& inAmbientColor, const glm::vec4& inDiffuseColor, const glm::vec4& inSpecularColor);
  float AmbientColor[4];   // 4 * 6 = 16 bytes
  float DiffuseColor[4]; // 4 * 4 = 16 bytes
  float SpecularColor[4]; // 4 * 6 = 16  bytes
  float DummyColor[4]; // 4 * 6 = 16  bytes
};	// 64


class GameObject
{
public:
  GameObject() = default;
  virtual ~GameObject();

  GameObject(const Transform& inTransform, const std::vector<Mesh>& inMeshes)
    : m_meshes(inMeshes), m_transform(inTransform)
  {
  }
  

  Transform& GetTransform() { return m_transform; }
  const std::vector<Mesh>& GetMeshes() const { return m_meshes; }
  
  void SetTransform(const Transform& inTransform) { m_transform = inTransform; }
  void SetMeshes(const std::vector<Mesh>& inMeshes) { m_meshes = inMeshes; }
  
  virtual void Init() {}
  virtual void Update() {}
  virtual void Shutdown() {}
protected:
  std::vector<Mesh> m_meshes;
  Transform m_transform;
  Material m_material;
};
