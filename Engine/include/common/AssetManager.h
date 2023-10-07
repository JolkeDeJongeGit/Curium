#pragma once
#include "graphics/Mesh.h"

namespace AssetManager
{
    Texture LoadTexture(std::string const& inFilePath);

    bool DoesFileExist(std::string const& inFilePath);

    std::vector<uint8_t> ReadBinaryFile(std::string const& inFilePath);
}