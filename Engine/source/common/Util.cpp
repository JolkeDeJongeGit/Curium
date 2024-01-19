#include "precomp.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>
#include <include/imgui.h>

bool DecomposeTransform(const glm::mat4& inTransform, glm::vec3& outTranslation, glm::vec3& outScale, glm::vec3& outRotation)
{
    using namespace glm;
    using T = float;

    mat4 LocalMatrix(inTransform);

    if (epsilonEqual(LocalMatrix[3][3], 0.f, epsilon<T>()))
        return false;

    if (
        epsilonNotEqual(LocalMatrix[0][3], 0.f, epsilon<T>()) ||
        epsilonNotEqual(LocalMatrix[1][3], 0.f, epsilon<T>()) ||
        epsilonNotEqual(LocalMatrix[2][3], 0.f, epsilon<T>()))
    {
        LocalMatrix[0][3] = LocalMatrix[1][3] = LocalMatrix[2][3] = 0.f;
        LocalMatrix[3][3] = 1.f;
    }

    outTranslation = vec3(LocalMatrix[3]);
    LocalMatrix[3] = vec4(0.f, 0.f, 0.f, LocalMatrix[3].w);

    vec3 Row[3]{};

    for (length_t i = 0; i < 3; ++i)
        for (length_t j = 0; j < 3; ++j)
            Row[i][j] = LocalMatrix[i][j];

    outScale.x = length(Row[0]);
    Row[0] = detail::scale(Row[0], 1.f);
    outScale.y = length(Row[1]);
    Row[1] = detail::scale(Row[1], 1.f);
    outScale.z = length(Row[2]);
    Row[2] = detail::scale(Row[2], 1.f);

    outRotation.y = asinf(-Row[0][2]);
    if (cosf(outRotation.y) != 0.f)
    {
        outRotation.x = atan2f(Row[1][2], Row[2][2]);
        outRotation.z = atan2f(Row[0][1], Row[0][0]);
    }
    else
    {
        outRotation.x = atan2f(-Row[2][0], Row[1][1]);
        outRotation.z = 0.f;
    }
    return true;
}

glm::vec4 TransformPoint(const glm::vec4& point, const glm::mat4& matrix)
{
    glm::vec4 out;

    out.x = point.x * matrix[0][0] + point.y * matrix[1][0] + point.z * matrix[2][0] + matrix[3][0];
    out.y = point.x * matrix[0][1] + point.y * matrix[1][1] + point.z * matrix[2][1] + matrix[3][1];
    out.z = point.x * matrix[0][2] + point.y * matrix[1][2] + point.z * matrix[2][2] + matrix[3][2];
    out.w = point.x * matrix[0][3] + point.y * matrix[1][3] + point.z * matrix[2][3] + matrix[3][3];

    return out;
}

glm::vec2 WorldToScreen(const glm::vec3& worldPos, const glm::mat4& matV, const glm::mat4& matP)
{
    glm::vec4 trans;
    glm::vec4 worldPosT = glm::vec4(worldPos, 1);

    ;
    glm::vec2 position = glm::vec2(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y);
    glm::vec2 size = glm::vec2(ImGui::GetWindowWidth(), ImGui::GetWindowHeight());

    trans = TransformPoint(worldPosT, (matP * matV));
    trans *= 0.5f / trans.w;
    trans += glm::vec4(0.5f, 0.5f, 0, 0);
    trans.y = 1.f - trans.y;
    trans.x *= size.x;
    trans.y *= size.y;
    trans.x += position.x;
    trans.y += position.y;
    return glm::vec2(trans.x, trans.y);
}

