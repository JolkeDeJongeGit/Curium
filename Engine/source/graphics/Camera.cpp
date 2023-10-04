#include "precomp.h"
#include "graphics/Camera.h"

Camera::Camera(const Transform& inTransform, const float inAspectRatio, const float inFov)
{
    m_objectTransform = inTransform;
    m_aspectRatio = inAspectRatio;
    m_fovDegrees = inFov;

    UpdateDirections();
    UpdateView();
    UpdateProjection(m_aspectRatio);
}

void Camera::UpdateView()
{
    m_view = glm::lookAt(m_objectTransform.GetPosition(), m_objectTransform.GetPosition() + m_objectTransform.GetForwardVector(), m_objectTransform.GetUpVector());
}

void Camera::UpdateProjection(const float inAspectRatio)
{
    m_aspectRatio = inAspectRatio;
    m_projection = glm::perspective(glm::radians(m_fovDegrees), m_aspectRatio, m_near, m_far);
}

void Camera::Rotate(const glm::vec2 inATrans)
{
    m_yaw -= inATrans.x;
    m_pitch -= inATrans.y;

    if (m_pitch > 89.0f) { m_pitch = 89.0f; }
    if (m_pitch < -89.0f) { m_pitch = -89.0f; }

    glm::vec3 forward;
    forward.x = -sinf(m_yaw) * cosf(m_pitch);
    forward.y = sinf(m_pitch);
    forward.z = -cosf(m_yaw) * cosf(m_pitch);
    m_objectTransform.SetForwardVector(forward);
    
    UpdateView();
}

void Camera::UpdateDirections()
{
    // Forward is calculated in rotation functions
    glm::normalize(m_objectTransform.GetForwardVector());

    // Cross product of forward and the world up
    m_objectTransform.SetRightVector(glm::cross(m_objectTransform.GetForwardVector(), glm::vec3(up)));
    glm::normalize(m_objectTransform.GetRightVector());

    // Cross product of right and forward
    m_objectTransform.SetUpVector(glm::cross(m_objectTransform.GetRightVector(), m_objectTransform.GetForwardVector()));
    glm::normalize(m_objectTransform.GetUpVector());
}

void Camera::ProcessMouseMovement(float inOffsetX, float inOffsetY)
{
    inOffsetX *= m_sensitivity;
    inOffsetY *= m_sensitivity;

    m_yaw   += inOffsetX;
    m_pitch += inOffsetY;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (m_pitch > 1.4f) { m_pitch = 1.4f; }
    if (m_pitch < -1.4f) { m_pitch = -1.4f; }

    glm::vec3 forward;
    forward.x = -sinf(m_yaw) * cosf(m_pitch);
    forward.y = sinf(m_pitch);
    forward.z = -cosf(m_yaw) * cosf(m_pitch);
    m_objectTransform.SetForwardVector(forward);
    
    UpdateDirections();
    UpdateView();
}

void Camera::ProcessKeyMovement(Direction inDirection, const float inDeltaTime)
{
    glm::vec3 position = m_objectTransform.GetPosition();
    
    const float velocity = m_movementSpeed * inDeltaTime;
    if (inDirection == Direction::FORWARD)
        position += m_objectTransform.GetForwardVector() * velocity;
    if (inDirection == Direction::BACKWARDS)
        position -= m_objectTransform.GetForwardVector() * velocity;
    if (inDirection == Direction::RIGHT)
        position -= m_objectTransform.GetRightVector() * velocity;
    if (inDirection == Direction::LEFT)
        position += m_objectTransform.GetRightVector() * velocity;
    
    if (inDirection == Direction::UP)
        position += glm::vec3(up) * velocity;
    if (inDirection == Direction::DOWN)
        position -= glm::vec3(up) * velocity;
    
    m_objectTransform.SetPosition(position);
    UpdateView();
}
