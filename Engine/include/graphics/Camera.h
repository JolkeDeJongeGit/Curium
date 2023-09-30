#pragma once
class Camera
{
public:
    Camera();

    [[nodiscard]] glm::vec3 GetPosition() const;
    [[nodiscard]] glm::vec3 Right() const;
    [[nodiscard]] glm::vec3 Up()  const;
    [[nodiscard]] glm::vec3 Forward() const;

    // Sets camera position
     void SetPosition(const glm::vec3 & inPosition);

    // Moves camera using world space axes
    void Move(const glm::vec3 & inDelta);

    // Moves camera relative to itself
    void MoveLeftRight(float inDelta);
    void MoveUpDown(float inDelta);
    void MoveForwardBackward(float inDelta);

    // Rotates camera by an angle around an axis
    void Rotate(float inAngle, const glm::vec3 & inAxis);

    // Performs pitch, yaw, roll by an angle
    void Pitch(float inAngle);
    void Yaw(float inAngle);
    void Roll(float inAngle);
    
    // Returns only the projection matrix
    [[nodiscard]] glm::mat4 Projection() const;

    // Returns only the translation matrix
    [[nodiscard]] glm::mat4 Translation() const;

    // Returns only the rotation matrix
    [[nodiscard]] glm::mat4 Rotation() const;

    // Returns the view matrix (view = rotation * translation)
    [[nodiscard]] glm::mat4 View() const;

    // Returns the camera transformation matrix (projection * rotation * translation = projection * view)
    [[nodiscard]] glm::mat4 Matrix() const;

    // Sets projection settings
    void SetFieldOfView(float inFieldOfView);
    void SetAspectRatio(float inAspectRatio);
    void SetNearFarPlanes(float inNear, float inFar);

private:
    glm::vec3 m_position = glm::vec3(0, 0, 0);
    glm::fquat m_orientation = glm::fquat(1, 0, 0, 0);

    float m_fov = 50.f;
    float m_aspect = 4.0f/3.0f;

    float m_nearPlane = 0.1f;
    float m_farPlane = 100.f;
};

inline glm::vec3 Camera::GetPosition() const
{
    return m_position;
}

inline glm::vec3 Camera::Right() const
{
    return glm::vec3(glm::row(Rotation(), 0));  // NOLINT(modernize-return-braced-init-list)
}

inline glm::vec3 Camera::Up() const
{
    return glm::vec3(glm::row(Rotation(), 1));  // NOLINT(modernize-return-braced-init-list)
}

inline glm::vec3 Camera::Forward() const
{
    return glm::vec3(glm::row(Rotation(), 2));  // NOLINT(modernize-return-braced-init-list)
}

inline void Camera::SetPosition(const glm::vec3& inPosition)
{
    m_position = inPosition;
}

inline void Camera::Move(const glm::vec3& inDelta)
{
    m_position += inDelta;
}

inline void Camera::MoveLeftRight(float inDelta)
{
    m_position -= inDelta * Right();
}

inline void Camera::MoveUpDown(float inDelta)
{
    m_position -= inDelta * Up();
}

inline void Camera::MoveForwardBackward(float inDelta)
{
    m_position -= inDelta * Forward();
}

inline void Camera::Rotate(const float inAngle, const glm::vec3& inAxis)
{
    const glm::fquat rot = glm::normalize(glm::angleAxis(inAngle, inAxis));
	
    m_orientation = m_orientation * rot;
}

inline void Camera::Pitch(const float inAngle)
{
    const glm::fquat rot = glm::normalize(glm::angleAxis(inAngle, Right()));

    m_orientation = m_orientation * rot;
}

inline void Camera::Yaw(const float inAngle)
{
    const glm::fquat rot = glm::normalize(glm::angleAxis(inAngle, Up()));

    m_orientation = m_orientation * rot;
}

inline void Camera::Roll(const float inAngle)
{
    const glm::fquat rot = glm::normalize(glm::angleAxis(inAngle, Forward()));

    m_orientation = m_orientation * rot;
}

inline glm::mat4 Camera::Projection() const
{
    return glm::perspective(m_fov, m_aspect, m_nearPlane, m_farPlane);
}

inline glm::mat4 Camera::Translation() const
{
    return glm::translate(glm::mat4(), -m_position);
}

inline glm::mat4 Camera::Rotation() const
{
    return glm::mat4_cast(m_orientation);
}

inline glm::mat4 Camera::View() const
{
    return Rotation() * Translation();
}

inline glm::mat4 Camera::Matrix() const
{
    return Projection() * View();
}

inline void Camera::SetFieldOfView(float inFieldOfView)
{
    m_fov = inFieldOfView;
}

inline void Camera::SetAspectRatio(float inAspectRatio)
{
    m_aspect = inAspectRatio;
}

inline void Camera::SetNearFarPlanes(float inNear, float inFar)
{
    m_nearPlane = inNear;
    m_farPlane = inFar;
}
