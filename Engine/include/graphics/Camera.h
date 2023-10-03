#pragma once
//class Camera
//{
//public:
//    Camera() = default;
//
//    [[nodiscard]] glm::vec3 GetPosition() const;
//    [[nodiscard]] glm::vec3 Right() const;
//    [[nodiscard]] glm::vec3 Up()  const;
//    [[nodiscard]] glm::vec3 Forward() const;
//
//    // Sets camera position
//     void SetPosition(const glm::vec3 & inPosition);
//
//    // Moves camera using world space axes
//    void Move(const glm::vec3 & inDelta);
//
//    // Moves camera relative to itself
//    void MoveLeftRight(float inDelta);
//    void MoveUpDown(float inDelta);
//    void MoveForwardBackward(float inDelta);
//
//    // Rotates camera by an angle around an axis
//    void Rotate(float inAngle, const glm::vec3 & inAxis);
//
//    // Performs pitch, yaw, roll by an angle
//    void Pitch(float inAngle);
//    void Yaw(float inAngle);
//    void Roll(float inAngle);
//    
//    // Returns only the projection matrix
//    [[nodiscard]] glm::mat4 Projection() const;
//
//    // Returns only the translation matrix
//    [[nodiscard]] glm::mat4 Translation() const;
//
//    // Returns only the rotation matrix
//    [[nodiscard]] glm::mat4 Rotation() const;
//
//    // Returns the view matrix (view = rotation * translation)
//    [[nodiscard]] glm::mat4 View() const;
//
//    // Returns the camera transformation matrix (projection * rotation * translation = projection * view)
//    [[nodiscard]] glm::mat4 Matrix() const;
//
//    // Sets projection settings
//    void SetFieldOfView(float inFieldOfView);
//    void SetAspectRatio(float inAspectRatio);
//    void SetNearFarPlanes(float inNear, float inFar);
//
//private:
//    glm::vec3 m_position = glm::vec3(0, 0, 0);
//    glm::fquat m_orientation = glm::fquat(1, 0, 0, 0);
//
//    float m_fov = 50.f;
//    float m_aspect = 4.0f/3.0f;
//
//    float m_nearPlane = 0.1f;
//    float m_farPlane = 100.f;
//};
//
//inline glm::vec3 Camera::GetPosition() const
//{
//    return m_position;
//}
//
//inline glm::vec3 Camera::Right() const
//{
//    return glm::vec3(glm::row(Rotation(), 0));  // NOLINT(modernize-return-braced-init-list)
//}
//
//inline glm::vec3 Camera::Up() const
//{
//    return glm::vec3(glm::row(Rotation(), 1));  // NOLINT(modernize-return-braced-init-list)
//}
//
//inline glm::vec3 Camera::Forward() const
//{
//    return glm::vec3(glm::row(Rotation(), 2));  // NOLINT(modernize-return-braced-init-list)
//}
//
//inline void Camera::SetPosition(const glm::vec3& inPosition)
//{
//    m_position = inPosition;
//}
//
//inline void Camera::Move(const glm::vec3& inDelta)
//{
//    m_position += inDelta;
//}
//
//inline void Camera::MoveLeftRight(float inDelta)
//{
//    m_position -= inDelta * Right();
//}
//
//inline void Camera::MoveUpDown(float inDelta)
//{
//    m_position -= inDelta * Up();
//}
//
//inline void Camera::MoveForwardBackward(float inDelta)
//{
//    m_position -= inDelta * Forward();
//}
//
//inline void Camera::Rotate(const float inAngle, const glm::vec3& inAxis)
//{
//    const glm::fquat rot = glm::normalize(glm::angleAxis(inAngle, inAxis));
//	
//    m_orientation = m_orientation * rot;
//}
//
//inline void Camera::Pitch(const float inAngle)
//{
//    const glm::fquat rot = glm::normalize(glm::angleAxis(inAngle, Right()));
//
//    m_orientation = m_orientation * rot;
//}
//
//inline void Camera::Yaw(const float inAngle)
//{
//    const glm::fquat rot = glm::normalize(glm::angleAxis(inAngle, Up()));
//
//    m_orientation = m_orientation * rot;
//}
//
//inline void Camera::Roll(const float inAngle)
//{
//    const glm::fquat rot = glm::normalize(glm::angleAxis(inAngle, Forward()));
//
//    m_orientation = m_orientation * rot;
//}
//
//inline glm::mat4 Camera::Projection() const
//{
//    return glm::perspective(m_fov, m_aspect, m_nearPlane, m_farPlane);
//}
//
//inline glm::mat4 Camera::Translation() const
//{
//    return glm::translate(glm::identity<glm::mat4>(), -m_position);
//}
//
//inline glm::mat4 Camera::Rotation() const
//{
//    return glm::mat4_cast(m_orientation);
//}
//
//inline glm::mat4 Camera::View() const
//{
//    return Rotation() * Translation();
//}
//
//inline glm::mat4 Camera::Matrix() const
//{
//    return Projection() * View();
//}
//
//inline void Camera::SetFieldOfView(float inFieldOfView)
//{
//    m_fov = inFieldOfView;
//}
//
//inline void Camera::SetAspectRatio(float inAspectRatio)
//{
//    m_aspect = inAspectRatio;
//}
//
//inline void Camera::SetNearFarPlanes(float inNear, float inFar)
//{
//    m_nearPlane = inNear;
//    m_farPlane = inFar;
//}


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

// Default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;


// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
    // camera Attributes
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    // euler Angles
    float Yaw;
    float Pitch;
    // camera options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

    // constructor with vectors
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }
    // constructor with scalar values
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = glm::vec3(posX, posY, posZ);
        WorldUp = glm::vec3(upX, upY, upZ);
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }

    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(Position, Position + Front, Up);
    }

    // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void ProcessKeyboard(Camera_Movement direction, float deltaTime)
    {
        float velocity = MovementSpeed * deltaTime;
        if (direction == FORWARD)
            Position += Front * velocity;
        if (direction == BACKWARD)
            Position -= Front * velocity;
        if (direction == LEFT)
            Position -= Right * velocity;
        if (direction == RIGHT)
            Position += Right * velocity;
    }

    // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true)
    {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Yaw += xoffset;
        Pitch += yoffset;

        // make sure that when pitch is out of bounds, screen doesn't get flipped
        if (constrainPitch)
        {
            if (Pitch > 89.0f)
                Pitch = 89.0f;
            if (Pitch < -89.0f)
                Pitch = -89.0f;
        }

        // update Front, Right and Up Vectors using the updated Euler angles
        updateCameraVectors();
    }

    // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void ProcessMouseScroll(float yoffset)
    {
        Zoom -= (float)yoffset;
        if (Zoom < 1.0f)
            Zoom = 1.0f;
        if (Zoom > 45.0f)
            Zoom = 45.0f;
    }

private:
    // calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors()
    {
        // calculate the new Front vector
        glm::vec3 front;
        front.x = cosf(glm::radians(Yaw)) * cosf(glm::radians(Pitch));
        front.y = sinf(glm::radians(Pitch));
        front.z = sinf(glm::radians(Yaw)) * cosf(glm::radians(Pitch));
        Front = glm::normalize(front);
        // also re-calculate the Right and Up vector
        Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        Up = glm::normalize(glm::cross(Right, Front));
    }
};