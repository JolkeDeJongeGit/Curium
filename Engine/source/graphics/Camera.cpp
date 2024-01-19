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

CameraData::CameraData(glm::mat4 const& view, glm::vec3 const& position)
{
    ViewProjectMatrix[0][0] = view[0][0];
    ViewProjectMatrix[0][1] = view[0][1];
    ViewProjectMatrix[0][2] = view[0][2];
    ViewProjectMatrix[0][3] = view[0][3];

    ViewProjectMatrix[1][0] = view[1][0];
    ViewProjectMatrix[1][1] = view[1][1];
    ViewProjectMatrix[1][2] = view[1][2];
    ViewProjectMatrix[1][3] = view[1][3];

    ViewProjectMatrix[2][0] = view[2][0];
    ViewProjectMatrix[2][1] = view[2][1];
    ViewProjectMatrix[2][2] = view[2][2];
    ViewProjectMatrix[2][3] = view[2][3];

    ViewProjectMatrix[3][0] = view[3][0];
    ViewProjectMatrix[3][1] = view[3][1];
    ViewProjectMatrix[3][2] = view[3][2];
    ViewProjectMatrix[3][3] = view[3][3];

    Eye[0] = position.x;
    Eye[1] = position.y;
    Eye[2] = position.z;

}