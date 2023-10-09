#include "precomp.h"
#include "common/AssetManager.h"

Texture AssetManager::LoadTexture(std::string const& inPath)
{
    PROFILE_FUNCTION();

    int width, height, channels;

    stbi_uc* data = stbi_load(path.c_str(), &width, &height, &channels, 0);



    stbi_image_free(data);

    return Texture();
}
