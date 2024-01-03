#include "precomp.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>
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
