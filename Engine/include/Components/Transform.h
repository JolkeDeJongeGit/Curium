﻿#pragma once
constexpr glm::vec4 forward = glm::vec4(0.f, 0.f, 1.f, 0.f);
constexpr glm::vec4 right = glm::vec4(1.f, 0.f, 0.f, 0.f);
constexpr glm::vec4 up = glm::vec4(0.f, 1.f, 0.f, 0.f);

class Transform
{
    glm::vec3 m_position = glm::vec3(0);
    glm::vec3 m_rotation = glm::vec3(0);
    glm::vec3 m_scale = glm::vec3(1);
    glm::vec3 m_forward = forward;
    glm::vec3 m_right = right;
    glm::vec3 m_up = up;
    mutable glm::mat4 m_model = glm::identity<glm::mat4>();

public:
    Transform();
    Transform(const glm::vec3& inPosition, const glm::vec3& inEulerRotation, const glm::vec3& inScale);
    Transform(const Transform&) = default;
    Transform(Transform&&) = default;

    void SetPosition(const glm::vec3& inPosition);
    void AddPosition(const glm::vec3& inPosition);
    void SetEulerRotation(const glm::vec3& inEulerAnglesInRadians);
    void SetScale(const glm::vec3& inScale);
    void Translate(const glm::vec3& inOffset);
    void Rotate(const glm::vec3& inRotation);
    void AddScale(const glm::vec3& inScale);
    void ScaleUniform(const float inScale);

    const glm::vec3& GetPosition() const {return m_position; }
    const glm::vec3& GetEulerRotation() const {return m_rotation; }
    const glm::vec3& GetScale() const {return m_scale; }
    const glm::vec3& GetForwardVector() const { return m_forward; }
    void SetForwardVector(const glm::vec3& inForward) { m_forward = inForward; }
    const glm::vec3& GetRightVector() const { return m_right; }
    void SetRightVector(const glm::vec3& inRight) { m_right = inRight; }
    const glm::vec3& GetUpVector() const { return m_up; }
    void SetUpVector(const glm::vec3& inUp) { m_up = inUp; }
    const glm::mat4& GetModelMatrix() const { UpdateModelMatrix(); return m_model; }

    Transform& operator = (const Transform&) = default;
    Transform& operator = (Transform&&) = default;
private:
    void UpdateModelMatrix() const;
    void CalculateOrientation();
};
