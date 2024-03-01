#include "precomp.h"
#include "Components/Transform.h"

Transform::Transform()
    : Transform(glm::vec3(0.f), glm::vec3(0.f), glm::vec3(1.f))
{
    UpdateModelMatrix();
}

Transform::Transform(const glm::vec3& inPosition, const glm::vec3& inEulerRotation, const glm::vec3& inScale)
    :
    m_position(inPosition),
    m_rotation(inEulerRotation),
    m_scale(inScale)
{
    CalculateOrientation();
    UpdateModelMatrix();
}

void Transform::SetPosition(const glm::vec3& inPosition)
{
    m_position = inPosition;
}

void Transform::AddPosition(const glm::vec3& inPosition)
{
    m_position += inPosition;
}

void Transform::SetEulerRotation(const glm::vec3& inEulerAnglesInRadians)
{
    m_rotation = inEulerAnglesInRadians;
    CalculateOrientation();
}

void Transform::SetScale(const glm::vec3& inScale)
{
    m_scale = inScale;
}

void Transform::Translate(const glm::vec3& inOffset)
{
    m_position += inOffset;
}

void Transform::Rotate(const glm::vec3& inRotation)
{
    m_rotation += inRotation;
}

void Transform::AddScale(const glm::vec3& inScale)
{
    m_scale += inScale;
}

void Transform::ScaleUniform(const float inScale)
{
    m_scale += glm::vec3(inScale);
}

void Transform::UpdateModelMatrix() const
{
    const glm::mat4 xRotate = glm::rotate(m_rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
    const glm::mat4 yRotate = glm::rotate(m_rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
    const glm::mat4 zRotate = glm::rotate(m_rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
    const glm::mat4 rotationMatrix = zRotate * yRotate * xRotate;

	m_model =  glm::translate(m_position) * rotationMatrix  * glm::scale(m_scale);
}

void Transform::CalculateOrientation()
{
    glm::mat4 rotationMatrix = glm::identity<glm::mat4>();
    rotationMatrix = glm::rotate(rotationMatrix, m_rotation.y, glm::vec3(right));
    rotationMatrix = glm::rotate(rotationMatrix, m_rotation.x, glm::vec3(up));
    rotationMatrix = glm::rotate(rotationMatrix, m_rotation.z, glm::vec3(forward));
    m_forward = glm::normalize(rotationMatrix * forward);
    m_right = glm::normalize(rotationMatrix * right);
    m_up = glm::normalize(rotationMatrix * up);
}
