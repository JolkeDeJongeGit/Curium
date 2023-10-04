#pragma once
#include "Transform.h"

class Mesh;

class GameObject
{
public:
  GameObject() = default;
  virtual ~GameObject() = default;

GameObject(const Transform& inTransform, const std::list<Mesh*>& inMeshes)
  : m_meshes(inMeshes), m_transform(inTransform){}
  Transform& GetTransform() { return m_transform; }
  std::list<Mesh*> GetMeshes() const { return m_meshes; }
  void SetTransform(const Transform& inTransform) { m_transform = inTransform; }
  void SetMeshes(const std::list<Mesh*>& inMeshes) { m_meshes = inMeshes; }
  
  virtual void Init() {}
  virtual void Update() {}
  virtual void Shutdown() {}
private:
  std::list<Mesh*> m_meshes;
  Transform m_transform;
};
