#pragma once
#include <winerror.h>
#include <exception>

inline void ThrowIfFailed(const HRESULT inHr)
{
    if (FAILED(inHr))
    {
        throw std::exception();
    }
}

bool DecomposeTransform(const glm::mat4& inTransform, glm::vec3& outTransform, glm::vec3& outScale, glm::vec3& outRotation);
glm::vec4 TransformPoint(const glm::vec4& point, const glm::mat4& matrix);
glm::vec2 WorldToScreen(const glm::vec3& worldPos, const glm::mat4& matV, const glm::mat4& matP);